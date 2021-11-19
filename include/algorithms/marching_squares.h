#pragma once
#include "common.h"
#include "square_grid.h"
#include "triangle_scene.h"
#include <array>


class MarchingSquares
{
private:
    // ordering of a square's vertices and edges :
    // v2--e2--v3
    // |       |
    // e1      e3
    // |       |
    // v1--e4--v4
    // to hash a vertex configuration 
    // (i.e. boolean values on the edges),
    // hash = 0b(v4)(v3)(v2)(v1).
    // table[hash] contains the list of all lines to add
    // for a given vertex configuration.
    // a line is given by the pair of edges it connects.
    typedef std::pair<int, int> line;
    static std::array<std::vector<line>, 16> table;


    void meshSquare(size_t x, size_t y);
    bool isInsideShape(size_t x, size_t y);

public:
    float (*m_density)(glm::vec2 pos);
    SquareGrid* m_grid;
    TriangleScene* m_triangleScene;


    MarchingSquares(float (*density)(glm::vec2 pos), SquareGrid* grid);
    // puts the result in m_triangleScene
    void triangulate();
};