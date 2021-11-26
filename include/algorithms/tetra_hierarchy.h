#pragma once
#include <unordered_map>
#include "algorithms/tetra.h"
#include "algorithms/diamond.h"



// The tetra hierarchy is built on a regular grid 
// with cells of unit size. The size of the grid
// is 1 + 2**maxDepth. This way we can use integers for coordinates.
class TetraHierarchy
{
public:
    TetraHierarchy(uint32_t maxDepth);
    ~TetraHierarchy();

private:
    // The valid depths range from 0 to maxDepth included.
    uint32_t m_maxDepth;
    // The valid coordinates range from 0 to maxCoord incuded.
    uint32_t m_maxCoord;
    // Diamonds indexed by their center vertex.
    // We only store diamonds that have an active tetra.
    std::unordered_map<vertex_t, Diamond*> m_diamonds;
    // Linked list of tetrahedrons
    Tetra* m_firstLeafTetra = nullptr;
    
    // TODO : use a memory pool for the tetras.

    void InitDiamond(Diamond* d);
    std::vector<vertex_t> InitChildren(Diamond* d);  
    std::vector<vertex_t> InitParents(Diamond* d);

    Diamond* FindOrCreateDiamond(const vertex_t& center);
    bool ShouldSplit(const Diamond* d);
    void CheckSplit(const Diamond* d, bool force);

    // We should only split leaf tetrahedrons.
    // Remove t from the leaf list and add both its children.
    // Add the children to their respective diamond (to find it,
    // compute the middle of the child's longest edge).
    void SplitTetra(Tetra* t);
    void AddToDiamond(Tetra* t);
    void AddLeaf(Tetra* t);
    void RemoveLeaf(Tetra* t);
    vertex_t VertexMidpoint(const vertex_t& v1, const vertex_t& v2);
    // Compute the indices (le1, le2) of the vertices
    // of the longest edge of t.
    // (i1, i2) are the indices of the other vertices.
    void FindLongestEdge(
        Tetra* t, 
        uint8_t* le1, 
        uint8_t* le2, 
        uint8_t* i1, 
        uint8_t* i2);
};
