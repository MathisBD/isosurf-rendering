#include "algorithms/tetra_hierarchy.h"
#include <assert.h>


inline Diamond* TetraHierarchy::FindOrCreateDiamond(const vertex_t& center) 
{
    auto it = m_diamonds.find(center);
    if (it == m_diamonds.end()) {
        Diamond* d = new Diamond(center);
        m_diamonds.insert({center, d});
        return d;
    }
    else {
        return it->second;
    }
}

void TetraHierarchy::CheckSplit(const Diamond* d, bool force) 
{
    if (!force && !ShouldSplit(d)) {
        return;
    }
    if (d->depth >= m_maxDepth) {
        return;
    }

    // ensure d is complete
    if (d->depth > 0) {
        for (const vertex_t& p : Parents(d)) {
            // it is okay to create a diamond pd
            // since we are going to add tetras to it anyways.
            Diamond* pd = FindOrCreateDiamond(p);
            if (!pd->IsComplete()) {
                CheckSplit(pd, true);
            }
        }    
    }
    assert(d->IsComplete());

    // actually split the diamond.
    // after this the diamond structure should still be sound.
    for (Tetra* t : d->activeTetras) {
        SplitTetra(t);
    }

    // recurse on children
    if (!force) {
        for (const vertex_t& c : Children(d)) {
            // this should not create a new diamond 
            // since we just split all the tetras in d.
            Diamond* cd = FindOrCreateDiamond(c);
            CheckSplit(cd, false);
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
    return { (v1.x+v2.x)/2, (v1.y+v2.y)/2, (v1.z+v2.z)/2 };    
}

static uint64_t DistanceSquared(const vertex_t& a, const vertex_t& b)
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
