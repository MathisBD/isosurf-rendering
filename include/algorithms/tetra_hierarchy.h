#pragma once
#include <unordered_map>
#include "algorithms/tetra.h"
#include "algorithms/diamond.h"
#include "algorithms/cube_grid.h"
#include "glm/gtx/hash.hpp"
#include "rendering/mesh.h"


// The tetra hierarchy is built on a regular grid 
// with cells of unit size. The size of the grid
// is 1 + 2**maxDepth. This way we can use integers for coordinates.
class TetraHierarchy
{
public:
    TetraHierarchy(uint32_t maxLevel, const CubeGrid& grid);    
    ~TetraHierarchy();
    
    // The split factor (range ]0, 1[) controls how much we split.
    // Values around 0.5 are reasonable. The higher the value, the more we split
    // diamonds close to the view origin and the less we split further away.
    void SplitAll(const glm::vec3& viewOrigin, float splitFactor);
    const Tetra* GetFirstLeafTetra() const;
    const Mesh& GetOutlineMesh() const;
    
    // The valid coordinates range from 0 to MaxCoord(maxLevel) incuded.
    inline static uint32_t MaxCoord(uint32_t maxLevel)
    {
        return (1U << (maxLevel+1));
    }
private:
    // The valid depths range from 0 to 3*maxLevel+2 included.
    uint32_t m_maxLevel;
    CubeGrid m_grid;
    glm::vec3 m_viewOrigin;
    float m_splitFactor;

    uint32_t m_checkID = 0;
    // Diamonds indexed by their center vertex.
    // We only store diamonds that have an active tetra.
    std::unordered_map<vertex_t, Diamond*> m_diamonds;
    Diamond* m_rootDiamond;
    // Linked list of tetrahedrons
    Tetra* m_firstLeafTetra = nullptr;
    // TODO : use a memory pool for the tetras.
    Mesh* m_outline = nullptr;

    void CreateRootDiamond();
    Diamond* FindOrCreateDiamond(const vertex_t& center);
    bool ShouldSplit(const Diamond* d);
    void CheckSplit(Diamond* d, uint32_t checkID);
    void ForceSplit(Diamond* d);

    // We should only split leaf tetrahedrons.
    // Remove t from the leaf list and add both its children.
    // Add the children to their respective diamond (to find it,
    // compute the middle of the child's longest edge).
    void SplitTetra(Tetra* t);
    void AddToDiamond(Tetra* t);
    void ComputeMesh(Tetra* t);
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

    void AddOutline(const Tetra* t);
};
