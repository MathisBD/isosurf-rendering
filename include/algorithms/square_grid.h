#pragma once
#include <glm/glm.hpp>
#include <vector>

class SquareGrid
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

    SquareGrid(size_t subdivs_x, size_t subdivs_y, 
        float left, float right, float bottom, float top);

    float Width();
    float Height();

    float SubdivisionWidth();
    float SubdivisionHeight();

    glm::vec2 WorldPosition(size_t x, size_t y);
};