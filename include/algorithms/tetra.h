#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "rendering/mesh.h"


typedef glm::u32vec3 vertex_t;

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
public:
    vertex_t vertices[4];

    struct _Tetra* children[2] { nullptr, nullptr };
    struct _Tetra* parent = nullptr;
    
    // leaf tetrahedrons are stored in a linked-list
    struct _Tetra* nextLeaf = nullptr;
    struct _Tetra* prevLeaf = nullptr;

    Mesh* mesh = nullptr;
} Tetra;