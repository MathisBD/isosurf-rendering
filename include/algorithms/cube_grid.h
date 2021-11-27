#pragma once
#include <glm/glm.hpp>
#include <vector>

struct CubeGrid
{
public:
    // number of vertices in the grid (including endpoints)
    glm::u32vec3 dim;
    // world position of the grid
    glm::vec3 lowVertex;
    glm::vec3 highVertex;
    // size of the grid
    glm::vec3 worldSize;
    glm::vec3 cellSize;

    CubeGrid(
        const glm::u32vec3& dim, 
        const glm::vec3& low,
        const glm::vec3& high) :
        dim(dim), 
        lowVertex(low), 
        highVertex(high),
        worldSize(high-low),
        cellSize({
            (high.x-low.x) / (dim.x-1), 
            (high.y-low.y) / (dim.y-1), 
            (high.z-low.z) / (dim.z-1)
        })
    {
    }
    
    inline glm::vec3 WorldPosition(glm::u32vec3 pos) const 
    {
        #define VEC_MUL(v1, v2) glm::vec3((v1).x*(v2).x, (v1).y*(v2).y, (v1).z*(v2).z)
        return lowVertex + VEC_MUL(pos, cellSize);    
    }
};