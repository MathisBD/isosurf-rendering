#pragma once
#include <unordered_map>
#include <glm/glm.hpp>
#include <vector>


typedef glm::u64vec3 vertex_t;


// The representation used for tetrahedrons and diamonds is a tradeoff
// between rendering speed / hierarchy split-merge speed. 
// The hierarchy overhead will likely be small compared to the 
// marching cubes computation time, so I chose to prioritize the rendering time.
// I thus chose a rather explicit representation (with lots of pointers) rather
// than an implicit representation (only store diamond centers essentially, and
// decode all other diamond/tetra info from the centers). This way I can have
// all the leaf tetra in a linked list, which should be very easy to traverse
// for rendering.
typedef struct _Tetra
{
    vertex_t vertices[4];
    // The middle of the tetrahdera's longest edge.
    // This is also the center of the tetrahedron's diamond.
    vertex_t splitVertex;
    struct _Tetra* children[2];
    struct _Tetra* parent;
    // leaf tetrahedrons are stored in a linked-list
    struct _Tetra* nextLeaf;
    struct _Tetra* prevLeaf;
} Tetra;


struct Diamond
{
public:
    // All the info can be retrieved from only the center position.
    // We cache it because there will likely be relatively few diamonds
    // (much less than triangles in the mesh).
    vertex_t center;
    uint8_t depth;
    uint8_t phase;
    uint8_t level;
    std::vector<Tetra*> activeTetras;

    inline bool IsComplete() 
    {
        switch (phase) {
        case 0: return activeTetras.size() >= 6;
        case 1: return activeTetras.size() >= 4;
        case 2: return activeTetras.size() >= 8;
        }
        assert(false);
        return false;
    }
};

// The tetra hierarchy is built on a regular grid 
// with cells of unit size. The size of the grid
// is 1 + 2**maxDepth. This way we can use integers for coordinates.
class TetraHierarchy
{
public:
    TetraHierarchy(uint32_t maxDepth);
    ~TetraHierarchy();

private:
    uint32_t m_maxDepth;
    // diamonds indexed by their center vertex.
    std::unordered_map<vertex_t, Diamond> m_diamonds;
    // linked list of tetrahedrons
    Tetra* m_firstLeafTetra = nullptr;
    // TODO : add a memory pool for the tetras.

    std::vector<vertex_t> DiamondChildren(const Diamond& d);  
    std::vector<vertex_t> DiamondParents(const Diamond& d);
    //std::vector<vertex_t[4]> Tetrahedrons(const Diamond& d); 
 
    void DiamondPredicateSplit(const Diamond& d, bool force);

    // Remove t from the leaf list and add both its children.
    // Add the children to their respective diamond (to find it,
    // compute the middle of the child's longest edge).
    void SplitTetra(Tetra* t);
};
