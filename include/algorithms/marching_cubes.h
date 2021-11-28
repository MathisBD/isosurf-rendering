#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "rendering/mesh.h"
#include "algorithms/hexa_grid.h"

typedef struct 
{
    glm::vec3 position;
    bool insideShape;
} MCVertex;

typedef struct
{
    // Does the isosurface cross this edge ?
    bool hasCrossing;
    // The index in the mesh of the isosurface vertex
    // on this edge. Only meaningful when hasCrossing==true.
    uint32_t isoVertexIdx;
} MCEdge;

typedef struct
{
    // position of the bottom left vertex
    uint32_t x, y, z;
    // indices of all the cell edges
    uint32_t edgesIndices[12];
} MCCell;

class MCChunk
{
public:
    // dim is the number of vertices along each dimension.
    // There are dim-1 cells along each dimension.
    MCChunk(const HexaGrid& grid, float (*density)(glm::vec3 position));
    ~MCChunk();

    const Mesh& GetMesh();
private:
    // Controls how many iterations to perform when
    // binary searching for the crossing position 
    // on an edge.
    const static uint32_t BIN_SEARCH_ITERATIONS = 5;

    glm::u32vec3 m_dim;
    float (*m_density)(glm::vec3 position);
    
    // the vertex at (x,y,z) is stored at Index3D(x,y,z).
    MCVertex* m_vertices = nullptr;
    
    // the edges are stored :
    //     (x,y,z)->(x+1,y,z) at 3*Index3D(x,y,z)
    //     (x,y,z)->(x,y+1,z) at 3*Index3D(x,y,z)+1
    //     (x,y,z)->(x,y,z+1) at 3*Index3D(x,y,z)+2
    MCEdge* m_edges = nullptr;
    
    Mesh* m_mesh = nullptr;

    inline uint32_t Index3D(uint32_t x, uint32_t y, uint32_t z);
    
    void LabelVertices(const HexaGrid& grid);
    void LabelEdges();
    void CalculateEdgeData(uint32_t v1, uint32_t v2, uint32_t direction);
    
    void Triangulate();
    uint8_t HashCell(const MCCell& cell);
    void CalculateCellEdges(MCCell& cell);
    void GenTriangles(const int triangles[16], const MCCell& cell);
};