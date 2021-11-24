#pragma once
#include <glm/glm.hpp>
#include <vector>

struct CubeGrid
{
public:
    // number of vertices in the grid (including endpoints)
    const glm::u32vec3 dim;
    // world position of the grid
    const glm::vec3 lowVertex;
    const glm::vec3 highVertex;
    // size of the grid
    const glm::vec3 worldSize;
    const glm::vec3 cellSize;

    CubeGrid(
        const glm::u32vec3& dim, 
        const glm::vec3& lowVertex,
        const glm::vec3& highVertex);

    glm::vec3 WorldPosition(glm::u32vec3 pos) const;
};