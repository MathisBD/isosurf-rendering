#include "applications/sandbox.h"
#include "algorithms/square_grid.h"
#include "algorithms/marching_squares.h"


Sandbox::Sandbox()
{

}

// a simple circle
float Density(glm::vec2 pos)
{
    return 3.0f - glm::length(pos);
}

void Sandbox::CreateScene() 
{
    glm::vec3 red = glm::vec3(1.0, 0.0, 0.0);
    glm::vec3 blue = glm::vec3(0.0, 0.0, 1.0);
    glm::vec3 green = glm::vec3(0.0, 1.0, 0.0);
    glm::vec3 white = glm::vec3(1.0, 1.0, 1.0);

    MarchingSquares::InitTable();
    auto grid = new SquareGrid(10, 10, -10.0, 10.0, -10.0, 10.0);
    auto ms = new MarchingSquares(&Density, grid);
    ms->Triangulate();

    // take the scene from the marching squares
    m_scene = ms->m_triangleScene;
    m_scene->SetCameraViewHeight(10.0f);
    m_scene->SetCameraTranslationSpeed(5.0f);

    // draw the grid lines
    float lineWidth = 0.02f; 
    for (size_t x = 0; x < grid->m_subdivs_x; x++) {
        m_scene->AddLine(
            glm::vec3(grid->WorldPosition(x, 0), 0.0f),
            glm::vec3(grid->WorldPosition(x, grid->m_subdivs_y-1), 0.0f),
            white,
            lineWidth);
    }
    for (size_t y = 0; y < grid->m_subdivs_y; y++) {
        m_scene->AddLine(
            glm::vec3(grid->WorldPosition(0, y), 0.0f),
            glm::vec3(grid->WorldPosition(grid->m_subdivs_x-1, y), 0.0f),
            white,
            lineWidth);
    }
}
