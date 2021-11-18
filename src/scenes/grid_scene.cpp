#include "scenes/grid_scene.h"
#include <assert.h>


GridScene::GridScene(size_t subdivs_x, size_t subdivs_y, 
    float left, float right, float bottom, float top) :
    m_subdivs_x(subdivs_x), m_subdivs_y(subdivs_y), m_left(left), 
    m_right(right), m_bottom(bottom), m_top(top)
{
    assert(subdivs_x >= 2);
    assert(subdivs_y >= 2);
    assert(left <= right);
    assert(bottom <= top);

    for (size_t i = 0; i < m_subdivs_x; i++) {
        m_grid.push_back(std::vector<float>(m_subdivs_y, 0.0f));
    }
}

float GridScene::width()
{
    return m_right - m_left;
}

float GridScene::height()
{
    return m_top - m_bottom;
}

float GridScene::subdivisionWidth()
{
    return width() / (m_subdivs_x - 1);
}

float GridScene::subdivisionHeight()
{
    return height() / (m_subdivs_y - 1);
}

glm::vec2 GridScene::worldPosition(size_t x, size_t y)
{
    assert(x < m_subdivs_x);
    assert(y < m_subdivs_y);
    
    return glm::vec2(
        m_left + x * subdivisionWidth(), 
        m_bottom + y * subdivisionHeight());
}