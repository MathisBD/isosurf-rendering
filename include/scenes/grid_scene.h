#pragma once
#include <glm/glm.hpp>
#include <vector>

class GridScene
{
public:
    // number of subdivisions in the grid (including endpoints)
    size_t m_subdivs_x;
    size_t m_subdivs_y;

    // world position of the grid
    float m_left;
    float m_right;
    float m_bottom;
    float m_top;

    // contents of the vertices in the grid, indexed as m_grid[x][y]
    std::vector<std::vector<float>> m_grid;

    GridScene(size_t subdivs_x, size_t subdivs_y, 
        float left, float right, float bottom, float top);

    float width();
    float height();

    float subdivisionWidth();
    float subdivisionHeight();

    glm::vec2 worldPosition(size_t x, size_t y);
};