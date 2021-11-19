#include "marching_squares.h"

auto MarchingSquares::table = {
    {}, // 0b0000
    {{1, 4}}, // 0b0001
    {}, // 0b0010
    {}, // 0b0011
    

};



MarchingSquares::MarchingSquares(float (*density)(glm::vec2 pos), SquareGrid* grid) :
    m_density(density), m_grid(grid)
{
    m_triangleScene = new TriangleScene();
}

void MarchingSquares::triangulate()
{
    for (size_t x = 0; x < m_grid->m_subdivs_x-1; x++) {
        for (size_t y = 0; y < m_grid->m_subdivs_y-1; y++) {
            meshSquare(x, y);
        }
    }
}


// mesh the square with bottom-left corner at (x, y)
void MarchingCubes::meshSquare(size_t x, size_t y)
{
    uint8_t hash = 0;
    if (isInsideShape(x, y)) {
        hash |= 1;
    }
    if (isInsideShape(x, y+1)) {
        hash |= 2;
    }
    if (isInsideShape(x+1, y+1)) {
        hash |= 4;
    }
    if (isInsideShape(x+1, y)) {
        hash |= 8;
    }

}

// is the point at (x, y) inside the shape ?
bool MarchingCubes::isInsideShape(size_t x, size_t y)
{
    glm::vec2 p = m_grid->worldPosition(x, y);
    return m_density(p) > 0; 
}