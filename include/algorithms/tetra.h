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
class Tetra
{
public:
    vertex_t vertices[4];

    Tetra* children[2];
    Tetra* parent;
    
    Tetra* nextLeaf;
    Tetra* prevLeaf;

    Mesh* mesh;

    // Root tetrahedron.
    Tetra()
    {
        children[0] = children[1] = nullptr;
        parent = nullptr;
        nextLeaf = prevLeaf = nullptr;
        mesh = nullptr;
    };

    Tetra(Tetra* p, uint8_t childIdx) 
    {
        children[0] = children[1] = nullptr;
        parent = p;
        nextLeaf = prevLeaf = nullptr;
        p->children[childIdx] = this;
        mesh = nullptr;
    };

    ~Tetra()
    {
        delete mesh;
    };
};