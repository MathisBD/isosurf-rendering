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
