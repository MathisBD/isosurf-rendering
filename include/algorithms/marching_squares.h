#pragma once
#include "common.h"
#include "square_grid.h"
#include "triangle_scene.h"
#include <array>


class MarchingSquares
{
private:
    // a line is given by the indices of the edges it connects.
    struct line {
        int firstEdgeIdx;
        int secondEdgeIdx;
    };
    // a vertex is given by its coordinates in the grid.
    struct vertex {
        int x;
        int y;
    };
    // an edge is given by its two endpoint vertices
    typedef std::pair<vertex, vertex> edge;
    // ordering of a square's vertices and edges :
    // v1--e1--v2
    // |       |
    // e0      e2
    // |       |
    // v0--e3--v3
    // to hash a vertex configuration 
    // (i.e. boolean values on the edges),
    // hash = 0b(v3)(v2)(v1)(v0).
    // table[hash] contains the list of all lines to add
    // for a given vertex configuration.
    inline static std::array<std::vector<line>, 16> s_table;

    void meshSquare(const vertex& v0);
    bool isInsideShape(const vertex& v);
    void buildLine(line line, const vertex& v0);
    edge edgeFromIndex(int edgeIndex, const vertex& v0);
    
    const static int binSearchIterations = 10;
    glm::vec2 surfacePoint(const edge& e);
public:
    float (*m_density)(glm::vec2 pos);
    SquareGrid* m_grid;
    TriangleScene* m_triangleScene;

    static void initTable();

    MarchingSquares(float (*density)(glm::vec2 pos), SquareGrid* grid);
    // puts the result in m_triangleScene
    void triangulate();
};