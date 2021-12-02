#include "algorithms/tetra_hierarchy.h"
#include <assert.h>
#include "algorithms/hexa_grid.h"
#include "algorithms/marching_cubes.h"
#include <stdio.h>
#include <chrono>


uint64_t MillisecondsSince(const std::chrono::high_resolution_clock::time_point& start) 
{
    auto curr = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(curr - start);
    return ms.count();    
}

void TetraHierarchy::SplitMerge(const glm::vec3& viewOrigin, uint32_t maxTimeMilliseconds) 
{
    // The view position changed : we have to check everything again.
    if (glm::distance(viewOrigin, m_viewOrigin) > 0.00001f) {
        m_viewOrigin = viewOrigin;
        m_splitQueue.SetCurrentToFirst();
        m_mergeQueue.SetCurrentToFirst();
    }
    // Merge
    auto start = std::chrono::high_resolution_clock::now();
    Diamond* d;
    while (d = m_mergeQueue.GetCurrent()) {
        m_mergeQueue.AdvanceCurrent();
        auto curr = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(curr - start);
        int64_t time = ms.count();
        if (time > (int64_t)maxTimeMilliseconds) {
            return;
        }
        if (ShouldMerge(d) && d->isSplit) {
            Merge(d);
        }
    } 
    // Split
    while (d = m_splitQueue.GetCurrent()) {
        m_splitQueue.AdvanceCurrent();
        if (MillisecondsSince(start) > maxTimeMilliseconds) {
            return;
        }
        if (ShouldSplit(d) && !d->isSplit) {
            Split(d);
        }
    }  
}

TetraHierarchy::TetraHierarchy(
    const CubeGrid& grid,
    float (*density)(glm::vec3 pos),
    const Parameters& params) :
    m_grid(grid.dim, grid.lowVertex, grid.worldSize),
    m_density(density),
    m_params(params),
    m_splitQueue(1),
    m_mergeQueue(2)
{
    uint32_t maxCoord = MaxCoord(m_params.maxLevel);
    assert(m_grid.dim == maxCoord + 1);

    Diamond* rootDiamond = CreateRootDiamond();
    m_splitQueue.AddFirst(rootDiamond);
}

TetraHierarchy::~TetraHierarchy() 
{
    // TODO : keep track of all the allocated 
    // tetras and diamonds, and free them all here.    
}

float TetraHierarchy::GoalLevel(const Diamond* d) 
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
    
    assert(0.0f < m_params.splitFactor && m_params.splitFactor < 1.0f);
    float goalLevel = glm::log(distance / maxDistance) / glm::log(m_params.splitFactor);
    return glm::max(goalLevel, 0.0f);
}

bool TetraHierarchy::ShouldSplit(const Diamond* d) 
{
    return d->Depth() < GetMaxDepth() &&
        (float)(d->level) < GoalLevel(d);
}

bool TetraHierarchy::ShouldMerge(const Diamond* d) 
{
    return (float)(d->level) >= (GoalLevel(d) + 1.0f);    
}

Diamond* TetraHierarchy::CreateRootDiamond()
{
    // Create the root diamond
    uint32_t maxCoord = MaxCoord(m_params.maxLevel);
    Diamond* rootDiamond = FindOrCreateDiamond({ maxCoord>>1, maxCoord>>1, maxCoord>>1 });
   
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
        //AddOutline(t);
        ComputeMesh(t);
    }
    return rootDiamond;
}

inline Diamond* TetraHierarchy::FindDiamond(const vertex_t& center) 
{
    auto it = m_diamonds.find(center);
    if (it == m_diamonds.end()) {
        return nullptr;
    }
    else {
        return it->second;
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

void TetraHierarchy::Split(Diamond* d)
{
    assert(!d->isSplit);
    // Ensure d is complete.
    for (const vertex_t& p : d->parents) {
        // It is okay to create a diamond pd
        // since we are going to add tetras to it anyways.
        Diamond* pd = FindOrCreateDiamond(p);
        if (!pd->isSplit) {
            Split(pd);
        }
    }    
    assert(d->IsComplete());

    // Update the split/merge qeues.
    m_splitQueue.Remove(d);
    // Add d to the beggining of the merge queue : no need
    // to check it again.
    m_mergeQueue.AddFirst(d);
    // Add children to the split queue.
    for (const vertex_t& c : d->children) {
        // It is okay to create a diamond here.
        Diamond* cd = FindOrCreateDiamond(c);
        assert(!cd->isSplit);
        // we want to check the child diamonds : add
        // them to the end of the split queue.
        assert(cd->queueID == m_splitQueue.GetID() ||
               cd->queueID == DiamondQueue::NO_QUEUE_ID);
        if (cd->queueID == DiamondQueue::NO_QUEUE_ID) {
            // If we are here, then cd has just been created.
            m_splitQueue.AddLast(cd);
        }
    }
    // Remove any parent that is in the merge queue.
    for (const vertex_t& p : d->parents) {
        Diamond* pd = FindDiamond(p);
        assert(pd);
        if (pd->queueID == m_mergeQueue.GetID()) {
            m_mergeQueue.Remove(pd);
        }
    }
    for (Tetra* t : d->activeTetras) {
        SplitTetra(t);
    }
    d->isSplit = true;
}

void TetraHierarchy::Merge(Diamond* d)
{
    assert(d->isSplit);
    assert(d->IsComplete());
    // Ensure d's children are merged.
    for (const vertex_t& c : d->children) {
        Diamond* cd = FindDiamond(c);
        assert(cd);
        if (cd->isSplit) {
            Merge(cd);
        }
    }    
    // Actually merge the diamond.
    // After this the diamond structure should still be sound.
    m_mergeQueue.Remove(d);
    // no need to check d again : add it to the beggining of the split queue.
    m_splitQueue.AddFirst(d);
    for (const vertex_t& p : d->parents) {
        Diamond* pd = FindDiamond(p);
        assert(pd);
        assert(pd->isSplit); 
        assert(pd->queueID == DiamondQueue::NO_QUEUE_ID);

        bool pdHasSplitChild = false;
        for (const vertex_t& c : pd->children) {
            Diamond* pcd = FindDiamond(c);
            assert(pcd);
            // TAKE CARE : we haven't marked d as not split yet,
            // but we should count it here !
            if (pcd->isSplit && pcd != d) {
                pdHasSplitChild = true;
                break;
            } 
        }
        if (!pdHasSplitChild) {
            m_mergeQueue.AddLast(pd);
        }
    }
    for (const vertex_t& c : d->children) {
        Diamond* cd = FindDiamond(c);
        assert(cd);
        assert(cd->queueID == m_splitQueue.GetID());
    }
    for (Tetra* t : d->activeTetras) {
        MergeTetra(t);
    }
    d->isSplit = false;
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
    //AddOutline(t0);
    ComputeMesh(t0);

    Tetra* t1 = new Tetra(t, 1);
    t1->vertices[0] = v[i1];
    t1->vertices[1] = v[i2];
    t1->vertices[2] = VertexMidpoint(v[le1], v[le2]);
    t1->vertices[3] = v[le2];
    AddToDiamond(t1);
    //AddOutline(t1);
    ComputeMesh(t1);
}

void TetraHierarchy::MergeTetra(Tetra* t) 
{
    Tetra* t0 = t->children[0]; 
    // t0 should be a leaf.
    assert(t0->children[0] == nullptr && t0->children[1] == nullptr); 
    RemoveFromDiamond(t0);
    delete t0; 

    Tetra* t1 = t->children[1]; 
    // t1 should be a leaf.
    assert(t1->children[0] == nullptr && t1->children[1] == nullptr); 
    RemoveFromDiamond(t1);
    delete t1; 

    t->children[0] = t->children[1] = nullptr;
}

void TetraHierarchy::AddToDiamond(Tetra* t) 
{
    uint8_t le1, le2, i1, i2;
    FindLongestEdge(t, &le1, &le2, &i1, &i2);
    const vertex_t sv = VertexMidpoint(t->vertices[le1], t->vertices[le2]);
    Diamond* d = FindDiamond(sv);
    assert(d);
    d->activeTetras.push_back(t);
}

void TetraHierarchy::RemoveFromDiamond(Tetra* t) 
{
    uint8_t le1, le2, i1, i2;
    FindLongestEdge(t, &le1, &le2, &i1, &i2);
    const vertex_t sv = VertexMidpoint(t->vertices[le1], t->vertices[le2]);
    Diamond* d = FindDiamond(sv);
    assert(d);
    assert(!d->isSplit);
    // remove the tetra.
    for (size_t i = 0; i < d->activeTetras.size(); i++) {
        if (d->activeTetras[i] == t) {
            d->activeTetras.erase(d->activeTetras.begin() + i);
            break;
        }
    }
    // delete the diamond if this was its last tetra.
    if (d->activeTetras.size() == 0) {
        m_splitQueue.Remove(d);
        m_diamonds.erase(d->center);
        delete d;
    }
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
        
        float alpha = t->depth / (float)GetMaxDepth();
        assert(0 <= alpha && alpha <= 1);
        const glm::vec3& color = glm::vec3(1, 1, 0) * (1 - alpha) + glm::vec3(1, 0, 1) * alpha;
        
        HexaGrid grid = HexaGrid(m_params.mcChunkDim, corners);
        MCChunk mc = MCChunk(grid, m_density, t->mesh, color);
        mc.Compute();
    }
    t->mesh->AllocateGPUBuffers(GL_STATIC_DRAW, t->mesh->GetVertexCount(), t->mesh->GetTriangleCount() * 3);
    t->mesh->UploadGPUBuffers();
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
    const Tetra* t, 
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

/*void TetraHierarchy::AddOutline(const Tetra* t) 
{
    glm::vec3 color = { 0, 0, 0 };
    uint32_t i0 = m_outline->AddVertex({ m_grid.WorldPosition(t->vertices[0]), color });
    uint32_t i1 = m_outline->AddVertex({ m_grid.WorldPosition(t->vertices[1]), color });
    uint32_t i2 = m_outline->AddVertex({ m_grid.WorldPosition(t->vertices[2]), color });
    uint32_t i3 = m_outline->AddVertex({ m_grid.WorldPosition(t->vertices[3]), color });
    m_outline->AddTriangle(i0, i1, i2);
    m_outline->AddTriangle(i0, i1, i3);
    m_outline->AddTriangle(i0, i2, i3);
    m_outline->AddTriangle(i1, i2, i3);
}*/