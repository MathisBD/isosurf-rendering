/*#pragma once
#include "square_grid.h"
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

    void MeshSquare(const vertex& v0);
    bool IsInsideShape(const vertex& v);
    void BuildLine(line line, const vertex& v0);
    edge EdgeFromIndex(int edgeIndex, const vertex& v0);
    
    const static int binSearchIterations = 10;
    glm::vec2 SurfacePoint(const edge& e);
public:
    float (*m_density)(glm::vec2 pos);
    CubeGrid* m_grid;
    Mesh* m_triangleScene;

    static void InitTable();

    MarchingSquares(float (*density)(glm::vec2 pos), CubeGrid* grid);
    // puts the result in m_triangleScene
    void Triangulate();
};*/