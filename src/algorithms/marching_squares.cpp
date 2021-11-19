#include "algorithms/marching_squares.h"

void MarchingSquares::initTable()
{
    s_table[0b0000] = {};
    s_table[0b0001] = {{0, 3}};
    s_table[0b0010] = {{0, 1}};
    s_table[0b0011] = {{1, 3}};

    s_table[0b0100] = {{1, 2}};
    s_table[0b0101] = {{0, 3}, {1, 2}};
    s_table[0b0110] = {{0, 2}};
    s_table[0b0111] = {{2, 3}};
        
    s_table[0b1000] = {{2, 3}};
    s_table[0b1001] = {{0, 2}};
    s_table[0b1010] = {{0, 1}, {2, 3}};
    s_table[0b1011] = {{1, 2}};
        
    s_table[0b1100] = {{1, 3}};
    s_table[0b1101] = {{0, 1}};
    s_table[0b1110] = {{0, 3}};
    s_table[0b1111] = {};
}


MarchingSquares::MarchingSquares(float (*density)(glm::vec2 pos), SquareGrid* grid) :
    m_density(density), m_grid(grid)
{
    m_triangleScene = new TriangleScene();
}

void MarchingSquares::triangulate()
{
    for (int x = 0; x < m_grid->m_subdivs_x-1; x++) {
        for (int y = 0; y < m_grid->m_subdivs_y-1; y++) {
            meshSquare({x, y});
        }
    }
}


// mesh the square with bottom-left corner at (x, y)
void MarchingSquares::meshSquare(const vertex& v0)
{
    int x = v0.x;
    int y = v0.y;

    uint8_t hash = 0;
    if (isInsideShape({x, y})) {
        hash |= 1;
    }
    if (isInsideShape({x, y+1})) {
        hash |= 2;
    }
    if (isInsideShape({x+1, y+1})) {
        hash |= 4;
    }
    if (isInsideShape({x+1, y})) {
        hash |= 8;
    }
    for (line line : s_table[hash]) {
        buildLine(line, v0);
    }
}

// is the point at (x, y) inside the shape ?
bool MarchingSquares::isInsideShape(const vertex& v)
{
    glm::vec2 p = m_grid->worldPosition(v.x, v.y);
    return m_density(p) > 0.0f; 
}

void MarchingSquares::buildLine(line line, const vertex& v0)
{
    glm::vec2 startPos = surfacePoint(edgeFromIndex(line.firstEdgeIdx, v0));
    glm::vec2 endPos = surfacePoint(edgeFromIndex(line.secondEdgeIdx, v0));
    
    glm::vec3 color(0, 0, 1);
    m_triangleScene->addLine(
        glm::vec3(startPos, 0), 
        glm::vec3(endPos, 0), 
        color, 
        0.02f);
}
 
MarchingSquares::edge MarchingSquares::edgeFromIndex(int edgeIndex, const vertex& v0)
{
    int x = v0.x;
    int y = v0.y;
    switch (edgeIndex) {
    case 0: return {{x, y}, {x, y+1}};
    case 1: return {{x, y+1}, {x+1, y+1}};
    case 2: return {{x+1, y+1}, {x+1, y}};
    case 3: return {{x+1, y}, {x, y}};
    default: assert(false);
    }
}

glm::vec2 MarchingSquares::surfacePoint(const edge& e)
{
    glm::vec2 start = m_grid->worldPosition(e.first.x, e.first.y);
    glm::vec2 end = m_grid->worldPosition(e.second.x, e.second.y);
            
    bool isStartInside = m_density(start) > 0;
    bool isEndInside = m_density(end) > 0;

    // binary search for the point on the surface (where density == 0).
    for (int i = 0; i < binSearchIterations; i++) {
        glm::vec2 middle = (start + end) / 2.0f;
        bool isMiddleInside = m_density(middle) > 0;

        if (isMiddleInside == isStartInside) {
            start = middle;
        }
        else {
            end = middle;
        }
    }
    return (start + end) / 2.0f;
}