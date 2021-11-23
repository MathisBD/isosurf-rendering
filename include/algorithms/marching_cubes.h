#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "rendering/mesh.h"


typedef struct 
{
    glm::vec3 position;
    float density;
} MCVertex;

typedef struct
{
    uint32_t firstVertexIdx;
    uint32_t secondVertexIdx;
    bool hasCrossing;
    glm::vec3 crossingPos;
} MCEdge;

typedef struct
{
    // index of the bottom left vertex
    uint32_t vertexIdx;
    // indices of all the cell edges
    uint32_t edgesIndices[12];
} MCCell;

class MCChunk
{
public:
    // dim is the number of vertices along each dimension.
    // There are dim-1 cells along each dimension.
    MCChunk(
        uint32_t dim, 
        glm::vec3*** vertexPositions, 
        float (*density)(glm::vec3 position));
    ~MCChunk();

    const Mesh& GetMesh();
private:
    // Controls how many iterations to perform when
    // binary searching for the crossing position 
    // on an edge.
    const static uint32_t BIN_SEARCH_ITERATIONS = 5;

    uint32_t m_dim;
    float (*m_density)(glm::vec3 position);
    
    // the vertex at (x,y,z) is stored at Index3D(x,y,z,m_dim).
    MCVertex* m_vertices;
    
    // the edges are stored :
    //     (x,y,z)->(x+1,y,z) at 3*Index3D(x,y,z,m_dim)
    //     (x,y,z)->(x,y+1,z) at 3*Index3D(x,y,z,m_dim)+1
    //     (x,y,z)->(x,y,z+1) at 3*Index3D(x,y,z,m_dim)+2
    MCEdge* m_edges;
    
    Mesh m_mesh;

    static inline uint32_t Index3D(uint32_t x, uint32_t y, uint32_t z, uint32_t dim);
    
    void LabelVertices(glm::vec3*** vertexPositions);
    void LabelCells();
    void LabelEdges();
    void CalculateEdgeData(MCEdge* edge);
    void LabelCells();
    
    void TriangulateCells();
};