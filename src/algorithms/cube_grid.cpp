#include <assert.h>
#include "algorithms/cube_grid.h"


CubeGrid::CubeGrid(
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

#define VEC_MUL(v1, v2) glm::vec3((v1).x*(v2).x, (v1).y*(v2).y, (v1).z*(v2).z)

glm::vec3 CubeGrid::WorldPosition(glm::u32vec3 pos) const 
{
    return lowVertex + VEC_MUL(pos, cellSize);    
}