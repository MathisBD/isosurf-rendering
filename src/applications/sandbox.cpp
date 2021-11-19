#include "applications/sandbox.h"
#include "scenes/grid_scene.h"

Sandbox::Sandbox()
{

}

// a simple circle
float density(glm::vec2 pos)
{
    return 3.0f - glm::length(pos);
}

GridScene* sampleGridScene()
{
    auto grid = new GridScene(20, 20, -5.0, 5.0, -5.0, 5.0);
    for (size_t x = 0; x < grid->m_subdivs_x; x++) {
        for (size_t y = 0; y < grid->m_subdivs_y; y++) {
            glm::vec2 pos = grid->worldPosition(x, y);
            grid->m_grid[x][y] = density(pos);
        }
    }
    return grid;
}

void Sandbox::createScene() 
{
    glm::vec3 red = glm::vec3(1.0, 0.0, 0.0);
    glm::vec3 blue = glm::vec3(0.0, 0.0, 1.0);
    glm::vec3 green = glm::vec3(0.0, 1.0, 0.0);
    glm::vec3 white = glm::vec3(1.0, 1.0, 1.0);

    auto grid = sampleGridScene(); 

    m_scene = new TriangleScene();
    m_scene->m_camera.m_viewHeight = 10.0f;
    m_scene->m_camera.m_translationSpeed = 5.0f;

    // draw the grid lines
    float lineWidth = 0.02f; 
    for (size_t x = 0; x < grid->m_subdivs_x; x++) {
        m_scene->addLine(
            glm::vec3(grid->worldPosition(x, 0), 0.0f),
            glm::vec3(grid->worldPosition(x, grid->m_subdivs_y-1), 0.0f),
            white,
            lineWidth);
    }
    for (size_t y = 0; y < grid->m_subdivs_y; y++) {
        m_scene->addLine(
            glm::vec3(grid->worldPosition(0, y), 0.0f),
            glm::vec3(grid->worldPosition(grid->m_subdivs_x-1, y), 0.0f),
            white,
            lineWidth);
    }
}
