#include "algorithms/square_grid.h"
#include <assert.h>


SquareGrid::SquareGrid(size_t subdivs_x, size_t subdivs_y, 
    float left, float right, float bottom, float top) :
    m_subdivs_x(subdivs_x), m_subdivs_y(subdivs_y), m_left(left), 
    m_right(right), m_bottom(bottom), m_top(top)
{
    assert(subdivs_x >= 2);
    assert(subdivs_y >= 2);
    assert(left <= right);
    assert(bottom <= top);
}

float SquareGrid::Width()
{
    return m_right - m_left;
}

float SquareGrid::Height()
{
    return m_top - m_bottom;
}

float SquareGrid::SubdivisionWidth()
{
    return Width() / (m_subdivs_x - 1);
}

float SquareGrid::SubdivisionHeight()
{
    return Height() / (m_subdivs_y - 1);
}

glm::vec2 SquareGrid::WorldPosition(size_t x, size_t y)
{
    assert(x < m_subdivs_x);
    assert(y < m_subdivs_y);
    
    return glm::vec2(
        m_left + x * SubdivisionWidth(), 
        m_bottom + y * SubdivisionHeight());
}