#include "algorithms/marching_cubes.h"

inline uint32_t MCChunk::Index3D(uint32_t x, uint32_t y, uint32_t z, uint32_t dim) 
{
    return x + dim * y + dim * dim * z; 
}

MCChunk::MCChunk(
    uint32_t dim, 
    glm::vec3*** vertexPositions, 
    float (*density)(glm::vec3 position)) :
    m_dim(dim), m_density(density)
{
    LabelVertices(vertexPositions);
    LabelCells();
    LabelEdges();
    TriangulateCells();
}

void MCChunk::LabelVertices(glm::vec3*** vertexPositions) 
{
    m_vertices = new MCVertex[m_dim*m_dim*m_dim];

    for (uint32_t x = 0; x < m_dim; x++) {
        for (uint32_t y = 0; y < m_dim; y++) {
            for (uint32_t z = 0; z < m_dim; z++) {
                MCVertex& v = m_vertices[Index3D(x, y, z)];
                v.position = vertexPositions[x][y][z];
                v.density = m_density(v.position);
            }
        }
    }
}


void MCChunk::LabelEdges() 
{
    m_edges = new MCEdge[3*m_dim*m_dim*m_dim];

    MCEdge* e = m_edges;
    for (uint32_t x = 0; x < m_dim; x++) {
        for (uint32_t y = 0; y < m_dim; y++) {
            for (uint32_t z = 0; z < m_dim; z++) {
                // x->x+1
                if (x < dim-1) {
                    e->firstVertexIdx = Index3D(x, y, z);
                    e->secondVertexIdx = Index3D(x+1, y, z);
                    CalculateEdgeData(e);

                    e++;
                }
                // y->y+1
                if (y < dim-1) {
                    e->firstVertexIdx = Index3D(x, y, z);
                    e->secondVertexIdx = Index3D(x, y+1, z);
                    CalculateEdgeData(e);
                    e++;
                }
                // z->z+1
                if (z < dim-1) {
                    e->firstVertexIdx = Index3D(x, y, z);
                    e->secondVertexIdx = Index3D(x, y, z+1);
                    CalculateEdgeData(e);
                    e++;
                }
            }
        }
    }   
}

void MCChunk::CalculateEdgeData(MCEdge* edge) 
{
    const MCVertex& first = m_vertices[edge->firstVertexIdx];
    const MCVertex& second = m_vertices[edge->secondVertexIdx]; 

    // does the edge have a crossing ?
    bool firstInside = first.density > 0;
    bool secondInside = second.density > 0;
    edge->hasCrossing = (firstInside != secondInside);

    // if yes, calculate the crossing position
    if (edge->hasCrossing) {
        glm::vec3 start = first.position;
        glm::vec3 end = second.position;
        // binary search
        for (int i = 0; i < BIN_SEARCH_ITERATIONS; i++) {
            glm::vec3 middle = (start + end) / 2.0f;
            bool middleInside = m_density(middle) > 0;

            if (middleInside == firstInside) {
                start = middle;
            }
            else {
                end = middle;
            }
        }
        edge->crossingPos = (start + end) / 2.0f;
    }
}
