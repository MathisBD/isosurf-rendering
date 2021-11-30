#pragma once
#include <unordered_map>
#include "algorithms/tetra.h"
#include "algorithms/diamond.h"
#include "algorithms/cube_grid.h"
#include "glm/gtx/hash.hpp"
#include "rendering/mesh.h"
#include <queue>


// TODO : use a memory pool for the tetras.
    
// The tetra hierarchy is built on a regular grid 
// with cells of unit size. The size of the grid
// is 1 + 2**maxDepth. This way we can use integers for coordinates.
class TetraHierarchy
{
public:
    struct Parameters
    {
    public:
        uint32_t maxLevel;
        uint32_t mcChunkDim;
        // The split factor (range ]0, 1[) controls how much we split.
        // Values around 0.5 are reasonable. The higher the value, the more we split
        // diamonds close to the view origin and the less we split further away.
        float splitFactor;
    };

    TetraHierarchy( 
        const CubeGrid& grid,
        float (*density)(glm::vec3 pos),
        const Parameters& params);    
    ~TetraHierarchy();
    
    void SplitMerge(const glm::vec3& viewOrigin, uint32_t maxTimeMilliseconds);
    
    inline const Diamond* GetFirstLeafDiamond() const { return m_firstLeafDiamond; }
    inline const Mesh& GetOutlineMesh() const { 
        m_outline->Build();
        return *m_outline; 
    }
    inline uint32_t GetMaxLevel() const { return m_params.maxLevel; }
    inline uint32_t GetMaxDepth() const { return 3*m_params.maxLevel + 2; }
    
    // The valid coordinates range from 0 to MaxCoord(maxLevel) incuded.
    inline static uint32_t MaxCoord(uint32_t maxLevel)
    {
        return (1U << (maxLevel+1));
    }
private:
    // The valid depths range from 0 to 3*maxLevel+2 included.
    CubeGrid m_grid;
    float (*m_density)(glm::vec3 pos);
    Parameters m_params;
    glm::vec3 m_viewOrigin;
    Mesh* m_outline = nullptr;

    // Diamonds indexed by their center vertex.
    // We only store diamonds that have an active tetra.
    std::unordered_map<vertex_t, Diamond*> m_diamonds;
    Diamond* m_rootDiamond = nullptr;
    Diamond* m_firstLeafDiamond = nullptr;

    // Split/Merge
    uint32_t m_checkID = 0;
    std::queue<Diamond*> m_splitQueue = {};
    //std::list<Diamond*> m_mergeQueue;


    void CreateRootDiamond();
    Diamond* FindOrCreateDiamond(const vertex_t& center);
    
    void RebuildSMQueues();
    bool ShouldSplit(const Diamond* d);
    void CheckSplit(Diamond* d);
    void ForceSplit(Diamond* d);
    //bool ShouldMerge(const Diamond* d);
    //void CheckMerge(Diamond* d, uint32_t checkID);
    
    // We should only split leaf tetrahedrons.
    // Remove t from the leaf list and add both its children.
    // Add the children to their respective diamond (to find it,
    // compute the middle of the child's longest edge).
    void SplitTetra(Tetra* t);
    //void MergeTetra(Tetra* t);
    void AddToDiamond(Tetra* t);
    //void RemoveFromDiamond(Tetra* t);
    void ComputeMesh(Tetra* t);
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
    void AddLeaf(Diamond* d);
    void RemoveLeaf(Diamond* d);
};
