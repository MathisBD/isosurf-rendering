#include "algorithms/tetra_hierarchy.h"
#include <assert.h>



void TetraHierarchy::SplitAll() 
{
    m_checkID++;
    CheckSplit(m_rootDiamond, m_checkID);    
    printf("Total diamond count=%lu\n", m_diamonds.size());
}

const Tetra* TetraHierarchy::GetFirstLeafTetra() const 
{
    return m_firstLeafTetra;
}

const Mesh& TetraHierarchy::GetOutlineMesh() const 
{
    m_outline->Build();
    return *m_outline;    
}


TetraHierarchy::TetraHierarchy(uint32_t maxLevel, const CubeGrid& grid) :
    m_grid(grid.dim, grid.lowVertex, grid.highVertex)
{
    m_maxLevel = maxLevel;
    m_checkID = 0;

    uint32_t maxCoord = MaxCoord(m_maxLevel);
    assert(m_grid.dim.x == maxCoord + 1);
    assert(m_grid.dim.y == maxCoord + 1);
    assert(m_grid.dim.z == maxCoord + 1);

    m_outline = new Mesh();

    CreateRootDiamond();
}

TetraHierarchy::~TetraHierarchy() 
{
    // TODO : keep track of all the allocated 
    // tetras and diamonds, and free them all here.    
}

bool TetraHierarchy::ShouldSplit(const Diamond* d) 
{
    return true;
}

void TetraHierarchy::CreateRootDiamond()
{
    // Create the root diamond
    uint32_t maxCoord = MaxCoord(m_maxLevel);
    m_rootDiamond = FindOrCreateDiamond({ maxCoord>>1, maxCoord>>1, maxCoord>>1 });
    // Create its 6 tetrahedra.
    const vertex_t cube_verts[8] = {
        { 0,        0,        0        },
        { 0,        maxCoord, 0        },
        { maxCoord, maxCoord, 0        },
        { maxCoord, 0,        0        },
        { 0,        0,        maxCoord },
        { 0,        maxCoord, maxCoord },
        { maxCoord, maxCoord, maxCoord },
        { maxCoord, 0,        maxCoord }
    };
    // This is the split edge of the root diamond.
    const uint8_t splitEdge[2] = { 0, 6 };
    const uint8_t tetraEdges[6][2] = {
        { 3, 7 },
        { 7, 4 },
        { 4, 5 },
        { 5, 1 },
        { 1, 2 },
        { 2, 3 }
    };
    for (uint8_t i = 0; i < 6; i++) {
        Tetra* t = new Tetra();
        t->vertices[0] = cube_verts[splitEdge[0]];
        t->vertices[1] = cube_verts[splitEdge[1]];
        t->vertices[2] = cube_verts[tetraEdges[i][0]];
        t->vertices[3] = cube_verts[tetraEdges[i][1]];
        t->parent = t->children[0] = t->children[1] = nullptr;
        AddLeaf(t);
        AddOutline(t);
        AddToDiamond(t);
    }
}

inline Diamond* TetraHierarchy::FindOrCreateDiamond(const vertex_t& center) 
{
    auto it = m_diamonds.find(center);
    if (it == m_diamonds.end()) {
        Diamond* d = new Diamond(center, m_maxLevel);
        m_diamonds.insert({center, d});
        return d;
    }
    else {
        return it->second;
    }
}

void TetraHierarchy::ForceSplit(Diamond* d)
{
    assert(d->level < m_maxLevel || d->phase < 2);
    assert(!d->isSplit);
    // Ensure d is complete.
    for (const vertex_t& p : d->parents) {
        // It is okay to create a diamond pd
        // since we are going to add tetras to it anyways.
        Diamond* pd = FindOrCreateDiamond(p);
        if (!pd->isSplit) {
            ForceSplit(pd);
        }
    }    
    assert(d->IsComplete());

    // Actually split the diamond.
    // After this the diamond structure should still be sound.
    for (Tetra* t : d->activeTetras) {
        SplitTetra(t);
    }
    d->isSplit = true;
}

void TetraHierarchy::CheckSplit(Diamond* d, uint32_t checkID) 
{
    assert(d->lastCheck < checkID);
    if (!ShouldSplit(d)) {
        return;
    }
    if (d->level == m_maxLevel && d->phase == 2) {
        return;
    }

    if (!d->isSplit) {
        // Ensure d is complete.
        for (const vertex_t& p : d->parents) {
            // It is okay to create a diamond pd
            // since we are going to add tetras to it anyways.
            Diamond* pd = FindOrCreateDiamond(p);
            if (!pd->isSplit) {
                ForceSplit(pd);
            }
        }    
        assert(d->IsComplete());

        // Actually split the diamond.
        // After this the diamond structure should still be sound.
        for (Tetra* t : d->activeTetras) {
            SplitTetra(t);
        }
        d->isSplit = true;
    }    
    d->lastCheck = checkID;

    // Recurse on children.
    for (const vertex_t& c : d->children) {
        // This should not create a new diamond 
        // since we just split all the tetras in d.
        Diamond* cd = FindOrCreateDiamond(c);
        if (cd->lastCheck < checkID) {
            CheckSplit(cd, checkID);
        }
    }
}

void TetraHierarchy::SplitTetra(Tetra* t) 
{
    // create the children
    uint8_t le1, le2, i1, i2;
    FindLongestEdge(t, &le1, &le2, &i1, &i2);
    const vertex_t* v = t->vertices;

    Tetra* t1 = new Tetra();
    t1->vertices[0] = v[i1];
    t1->vertices[1] = v[i2];
    t1->vertices[2] = VertexMidpoint(v[le1], v[le2]);
    t1->vertices[3] = v[le1];
    t1->children[0] = t1->children[1] = nullptr;
    t1->parent = t;
    t->children[0] = t1;
    
    Tetra* t2 = new Tetra();
    t2->vertices[0] = v[i1];
    t2->vertices[1] = v[i2];
    t2->vertices[2] = VertexMidpoint(v[le1], v[le2]);
    t2->vertices[3] = v[le2];
    t2->children[0] = t2->children[1] = nullptr;
    t2->parent = t;
    t->children[1] = t2;
    // Update the leaf list. 
    RemoveLeaf(t);
    AddLeaf(t1);
    AddLeaf(t2);
    // Add each child to its diamond.
    AddToDiamond(t1);
    AddToDiamond(t2);
    // Add the tetrahedron outlines to the mesh.
    AddOutline(t1);
    AddOutline(t2);
}

void TetraHierarchy::AddToDiamond(Tetra* t) 
{
    uint8_t le1, le2, i1, i2;
    FindLongestEdge(t, &le1, &le2, &i1, &i2);
    const vertex_t sv = VertexMidpoint(t->vertices[le1], t->vertices[le2]);
    Diamond* d = FindOrCreateDiamond(sv);
    d->activeTetras.push_back(t);
}

void TetraHierarchy::AddLeaf(Tetra* t) 
{
    if (!m_firstLeafTetra) {
        m_firstLeafTetra = t;
        t->nextLeaf = t->prevLeaf = nullptr;
        return;
    }
    t->nextLeaf = m_firstLeafTetra;
    t->prevLeaf = nullptr;
    m_firstLeafTetra->prevLeaf = t;
    m_firstLeafTetra = t;
}

void TetraHierarchy::RemoveLeaf(Tetra* t) 
{
    if (m_firstLeafTetra == t)
    {
        m_firstLeafTetra = t->nextLeaf;
    }    

    if (t->nextLeaf) {
        t->nextLeaf->prevLeaf = t->prevLeaf;
    }
    if (t->prevLeaf) {
        t->prevLeaf->nextLeaf = t->nextLeaf;
    }
    t->prevLeaf = t->nextLeaf = nullptr;
}

inline vertex_t TetraHierarchy::VertexMidpoint(const vertex_t& v1, const vertex_t& v2) 
{
    // We should only have to compute integer vertices ;
    // otherwise, the grid size is wrong.
    assert(((v1.x + v2.x) & 1) == 0);
    assert(((v1.y + v2.y) & 1) == 0);
    assert(((v1.z + v2.z) & 1) == 0);
    return { (v1.x+v2.x) >> 1, (v1.y+v2.y) >> 1, (v1.z+v2.z) >> 1 };    
}

inline static uint64_t DistanceSquared(const vertex_t& a, const vertex_t& b)
{
    uint64_t dx = a.x >= b.x ? (a.x - b.x) : (b.x - a.x);
    uint64_t dy = a.y >= b.y ? (a.y - b.y) : (b.y - a.y);
    uint64_t dz = a.z >= b.z ? (a.z - b.z) : (b.z - a.z);
    return dx*dx + dy*dy + dz*dz;
}


void TetraHierarchy::FindLongestEdge(
    Tetra* t, 
    uint8_t* le1, 
    uint8_t* le2, 
    uint8_t* i1, 
    uint8_t* i2) 
{
    // All the candidate (le1, le2, i1, i2).
    uint8_t edges[][4] = {
        { 0, 1,   2, 3 },
        { 0, 2,   1, 3 },
        { 0, 3,   1, 2 },
        { 1, 2,   0, 3 },
        { 1, 3,   0, 2 },
        { 2, 3,   0, 1 }
    };
    uint64_t maxLengthSquared = 0;
    const vertex_t* vs = t->vertices;
    for (size_t i = 0; i < 6; i++) {
        const uint8_t* e = edges[i];
        uint64_t lengthSquared = DistanceSquared(vs[e[0]], vs[e[1]]);
        if (lengthSquared >= maxLengthSquared) {
            *le1 = e[0];
            *le2 = e[1];
            *i1  = e[2];
            *i2  = e[3];
            maxLengthSquared = lengthSquared;
        }
    }
}

void TetraHierarchy::AddOutline(const Tetra* t) 
{
    glm::vec3 color { 0, 1, 1 };
    uint32_t i0 = m_outline->AddVertex(m_grid.WorldPosition(t->vertices[0]), color);
    uint32_t i1 = m_outline->AddVertex(m_grid.WorldPosition(t->vertices[1]), color);
    uint32_t i2 = m_outline->AddVertex(m_grid.WorldPosition(t->vertices[2]), color);
    uint32_t i3 = m_outline->AddVertex(m_grid.WorldPosition(t->vertices[3]), color);
    m_outline->AddTriangle(i0, i1, i2);
    m_outline->AddTriangle(i0, i1, i3);
    m_outline->AddTriangle(i0, i2, i3);
    m_outline->AddTriangle(i1, i2, i3);
}