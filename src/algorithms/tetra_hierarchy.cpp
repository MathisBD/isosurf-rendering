#include "algorithms/tetra_hierarchy.h"
#include <assert.h>
#include "algorithms/hexa_grid.h"
#include "algorithms/marching_cubes.h"
#include <stdio.h>
#include <chrono>



void TetraHierarchy::RebuildSMQueues() 
{
    assert(m_splitQueue.empty());
    //assert(m_mergeQueue.empty());

    Diamond* leaf = m_firstLeafDiamond;
    while (leaf) {
        m_splitQueue.push(leaf);
        leaf = leaf->nextLeaf;
    }    
}

void TetraHierarchy::SplitMerge(const glm::vec3& viewOrigin, uint32_t maxTimeMilliseconds) 
{
    // The view position changed : we have to check everything again.
    if (m_checkID == 0 || glm::distance(viewOrigin, m_viewOrigin) > 0.001f) {
        m_viewOrigin = viewOrigin;
        m_checkID++;
        RebuildSMQueues();
    }

    // Split
    auto start = std::chrono::high_resolution_clock::now();
    while (!m_splitQueue.empty()) {
        Diamond* d = m_splitQueue.front();
        m_splitQueue.pop();

        assert(d->lastCheck <= m_checkID);
        if (d->lastCheck < m_checkID) {
            CheckSplit(d);
        }

        // Did we run out of time ?
        auto curr = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(curr - start);
        if (ms.count() > maxTimeMilliseconds) {
            return;
        }
    }  
    // TODO : merge 
}


TetraHierarchy::TetraHierarchy(
    const CubeGrid& grid,
    float (*density)(glm::vec3 pos),
    const Parameters& params) :
    m_grid(grid.dim, grid.lowVertex, grid.worldSize),
    m_density(density),
    m_params(params),
    m_checkID(0)
{
    uint32_t maxCoord = MaxCoord(m_params.maxLevel);
    assert(m_grid.dim == maxCoord + 1);

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
    // Calculate the diamond's bounding sphere radius.
    float radius = m_grid.cellSize * (1U << (d->scale+1));
    switch (d->phase) {
    case 0: radius *= glm::sqrt(3); break;
    case 1: radius *= glm::sqrt(2); break;
    case 2: break;
    default: assert(false);
    }
    // Calculate the distance from the diamond's bounding sphere
    // to the view origin.
    float distance = glm::distance(m_viewOrigin, m_grid.WorldPosition(d->center));
    distance = glm::max(distance - radius, 0.1f);

    float maxDistance = glm::sqrt(3) * m_grid.worldSize;
    assert(distance < maxDistance);

    assert(0.0f < m_params.splitFactor && m_params.splitFactor < 1.0f);
    float goalLevel = glm::log(distance / maxDistance) / glm::log(m_params.splitFactor);
    
    return (float)(d->level) < goalLevel;
}

void TetraHierarchy::CreateRootDiamond()
{
    // Create the root diamond
    uint32_t maxCoord = MaxCoord(m_params.maxLevel);
    m_rootDiamond = FindOrCreateDiamond({ maxCoord>>1, maxCoord>>1, maxCoord>>1 });
    AddLeaf(m_rootDiamond);

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
        AddToDiamond(t);
        AddOutline(t);
        ComputeMesh(t);
    }
}

inline Diamond* TetraHierarchy::FindOrCreateDiamond(const vertex_t& center) 
{
    auto it = m_diamonds.find(center);
    if (it == m_diamonds.end()) {
        Diamond* d = new Diamond(center, m_params.maxLevel);
        m_diamonds.insert({center, d});
        return d;
    }
    else {
        return it->second;
    }
}

void TetraHierarchy::ForceSplit(Diamond* d)
{
    assert(d->level < m_params.maxLevel || d->phase < 2);
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
    RemoveLeaf(d);
    for (const vertex_t& c : d->children) {
        Diamond* cd = FindOrCreateDiamond(c);
        AddLeaf(cd);
    }
    d->isSplit = true;
}

void TetraHierarchy::CheckSplit(Diamond* d) 
{
    assert(d->lastCheck < m_checkID);
    if (!ShouldSplit(d)) {
        return;
    }
    if (d->level == m_params.maxLevel && d->phase == 2) {
        return;
    }

    if (!d->isSplit) {
        ForceSplit(d);
    }    
    d->lastCheck = m_checkID;

    // Recurse on children.
    for (const vertex_t& c : d->children) {
        // This should not create a new diamond 
        // since we just split all the tetras in d.
        Diamond* cd = FindOrCreateDiamond(c);
        if (cd->lastCheck < m_checkID) {
            m_splitQueue.push(cd);
        }
    }
}


void TetraHierarchy::SplitTetra(Tetra* t) 
{
    uint8_t le1, le2, i1, i2;
    FindLongestEdge(t, &le1, &le2, &i1, &i2);
    const vertex_t* v = t->vertices;

    Tetra* t0 = new Tetra(t, 0);
    t0->vertices[0] = v[i1];
    t0->vertices[1] = v[i2];
    t0->vertices[2] = VertexMidpoint(v[le1], v[le2]);
    t0->vertices[3] = v[le1];
    AddToDiamond(t0);
    AddOutline(t0);
    ComputeMesh(t0);

    Tetra* t1 = new Tetra(t, 1);
    t1->vertices[0] = v[i1];
    t1->vertices[1] = v[i2];
    t1->vertices[2] = VertexMidpoint(v[le1], v[le2]);
    t1->vertices[3] = v[le2];
    AddToDiamond(t1);
    AddOutline(t1);
    ComputeMesh(t1);
}

void TetraHierarchy::AddToDiamond(Tetra* t) 
{
    uint8_t le1, le2, i1, i2;
    FindLongestEdge(t, &le1, &le2, &i1, &i2);
    const vertex_t sv = VertexMidpoint(t->vertices[le1], t->vertices[le2]);
    Diamond* d = FindOrCreateDiamond(sv);
    d->activeTetras.push_back(t);
}

void TetraHierarchy::ComputeMesh(Tetra* t) 
{
    // The first index is the corner we are processing,
    // then the other three vertices.
    uint8_t hexas[][4] = 
    {
        { 0,    1, 2, 3 },
        { 1,    0, 2, 3 },
        { 2,    1, 0, 3 },
        { 3,    1, 2, 0 },
    };

    assert(!t->mesh);
    t->mesh = new Mesh();
    for (uint8_t i = 0; i < 4; i++) {
        const uint8_t* h = hexas[i]; 
        // Compute the mesh for the hexahedron located at corner h[0].
        glm::vec3 v0 = m_grid.WorldPosition(t->vertices[h[0]]);
        glm::vec3 v1 = m_grid.WorldPosition(t->vertices[h[1]]);
        glm::vec3 v2 = m_grid.WorldPosition(t->vertices[h[2]]);
        glm::vec3 v3 = m_grid.WorldPosition(t->vertices[h[3]]);

        glm::vec3 corners[8];
        corners[0b000] = v0;
        corners[0b100] = (v0 + v1) / 2.0f;
        corners[0b010] = (v0 + v2) / 2.0f;
        corners[0b001] = (v0 + v3) / 2.0f;
        corners[0b110] = (v0 + v1 + v2) / 3.0f;
        corners[0b011] = (v0 + v2 + v3) / 3.0f;
        corners[0b101] = (v0 + v1 + v3) / 3.0f;
        corners[0b111] = (v0 + v1 + v2 + v3) / 4.0f;

        HexaGrid grid = HexaGrid(m_params.mcChunkDim, corners);
        MCChunk mc = MCChunk(grid, m_density, t->mesh);
        mc.Compute();
    }
    t->mesh->Build();
}

// d is allowed to already be a leaf.
void TetraHierarchy::AddLeaf(Diamond* d) 
{
    // d is already a leaf
    if (d->nextLeaf || d->prevLeaf) {
        return;
    }

    if (!m_firstLeafDiamond) {
        m_firstLeafDiamond = d;
        d->nextLeaf = d->prevLeaf = nullptr;
        return;
    }
    d->nextLeaf = m_firstLeafDiamond;
    d->prevLeaf = nullptr;
    m_firstLeafDiamond->prevLeaf = d;
    m_firstLeafDiamond = d;
}

// Assumes d is already a leaf.
void TetraHierarchy::RemoveLeaf(Diamond* d) 
{
    assert(d->nextLeaf || d->prevLeaf);
    
    if (m_firstLeafDiamond == d)
    {
        m_firstLeafDiamond = d->nextLeaf;
    }    

    if (d->nextLeaf) {
        d->nextLeaf->prevLeaf = d->prevLeaf;
    }
    if (d->prevLeaf) {
        d->prevLeaf->nextLeaf = d->nextLeaf;
    }
    d->prevLeaf = d->nextLeaf = nullptr;
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

// TODO : store the edge distances (not squared? -> if all integer) 
// directly in the tetrahedrons, to avoid calculating them all the time.
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
    uint32_t i0 = m_outline->AddVertex(m_grid.WorldPosition(t->vertices[0]));
    uint32_t i1 = m_outline->AddVertex(m_grid.WorldPosition(t->vertices[1]));
    uint32_t i2 = m_outline->AddVertex(m_grid.WorldPosition(t->vertices[2]));
    uint32_t i3 = m_outline->AddVertex(m_grid.WorldPosition(t->vertices[3]));
    m_outline->AddTriangle(i0, i1, i2);
    m_outline->AddTriangle(i0, i1, i3);
    m_outline->AddTriangle(i0, i2, i3);
    m_outline->AddTriangle(i1, i2, i3);
}