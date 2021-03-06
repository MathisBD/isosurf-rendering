#pragma once
#include <unordered_map>
#include "algorithms/tetra.h"
#include "algorithms/diamond.h"
#include "algorithms/cube_grid.h"
#include "glm/gtx/hash.hpp"
#include "rendering/mesh.h"
#include "algorithms/diamond_queue.h"
#include "rendering/plane.h"
#include <mutex>
#include <condition_variable>
#include "rendering/renderer.h"
#include "thread_pool.h"
#include <algorithm>


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
        float maxDistance;
    };

    TetraHierarchy( 
        const CubeGrid& grid,
        float (*density)(glm::vec3 pos),
        const Parameters& params);    
    ~TetraHierarchy();
    
    // Do the split/merge process.
    void Work();
    void UpdateCamera(
        const glm::vec3& viewOrigin,
        const Plane frustrumPlanes[6]);
    
    // Calls f on the mesh of all leaf tetras.
    void ForEveryMesh(const std::function<void(const Mesh&)>& f);
    
    uint32_t GetMaxLevel() const;
    uint32_t GetMaxDepth() const;
    // The valid coordinates range from 0 to MaxCoord(maxLevel) incuded.
    static uint32_t MaxCoord(uint32_t maxLevel);
private:
    // synchronization variables
    std::mutex m_mutex;
    std::condition_variable m_onCameraChanged;
    ThreadPool m_threadPool;

    // The valid depths range from 0 to 3*maxLevel+2 included.
    CubeGrid m_grid;
    float (*m_density)(glm::vec3 pos);
    Parameters m_params;
    glm::vec3 m_viewOrigin;
    const Plane* m_frustrumPlanes;

    // Diamonds indexed by their center vertex.
    // We only store diamonds that have an active tetra.
    std::unordered_map<vertex_t, Diamond*> m_diamonds;
    // The split queue holds all leaf diamonds,
    // i.e. diamonds that are not split.
    DiamondQueue m_splitQueue;
    // The merge queue holds all diamonds that are split
    // but have no split child.
    DiamondQueue m_mergeQueue;
    // This is incremented every time we start a new 
    // split/merge phase, i.e. when the camera position or rotation is updated.
    uint32_t m_checkID = 0;

    Diamond* CreateRootDiamond();
    // Returns a nullptr if the diamond wasn't found.
    Diamond* FindDiamond(const vertex_t& center);
    Diamond* FindOrCreateDiamond(const vertex_t& center);

    // Try to perform a single split.
    // Returns true if successful.
    bool TrySplit();
    // Try to perform a single merge.
    // Returns true if successful.
    bool TryMerge();
    void Split(Diamond* d);
    void Merge(Diamond* d);
    
    float GoalLevel(const Diamond* d);
    bool ShouldMerge(const Diamond* d);
    bool ShouldSplit(const Diamond* d);
    
    // We should only split leaf tetrahedrons.
    // Remove t from the leaf list and add both its children.
    // Add the children to their respective diamond (to find it,
    // compute the middle of the child's longest edge).
    void SplitTetra(Tetra* t, std::vector<std::future<void>>& futures);
    void MergeTetra(Tetra* t);
    void AddToDiamond(Tetra* t);
    void RemoveFromDiamond(Tetra* t);
    void ComputeMesh(Tetra* t);
    vertex_t VertexMidpoint(const vertex_t& v1, const vertex_t& v2);
    // Compute the indices (le1, le2) of the vertices
    // of the longest edge of t.
    // (i1, i2) are the indices of the other vertices.
    void FindLongestEdge(
        const Tetra* t, 
        uint8_t* le1, 
        uint8_t* le2, 
        uint8_t* i1, 
        uint8_t* i2);

    bool IsInViewFrustrum(const Diamond* d);

    //void AddOutline(const Tetra* t);
};
