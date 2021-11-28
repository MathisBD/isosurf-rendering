#include "algorithms/marching_cubes.h"
#include "algorithms/mc_tables.h"



inline uint32_t MCChunk::Index3D(uint32_t x, uint32_t y, uint32_t z) 
{
    return x + m_dim.x * y + m_dim.x * m_dim.y * z; 
}

MCChunk::MCChunk(const HexaGrid& grid, float (*density)(glm::vec3 position)) :
    m_dim(grid.dim), m_density(density)
{
    m_mesh = new Mesh();
    LabelVertices(grid);
    LabelEdges();
    Triangulate();
    m_mesh->Build();
}

MCChunk::~MCChunk() 
{
    delete m_mesh;
    delete m_vertices;
    delete m_edges;  
}

const Mesh& MCChunk::GetMesh() 
{
    return *m_mesh;
}

void MCChunk::LabelVertices(const HexaGrid& grid) 
{
    m_vertices = new MCVertex[m_dim.x*m_dim.y*m_dim.z];

    for (uint32_t x = 0; x < m_dim.x; x++) {
        for (uint32_t y = 0; y < m_dim.y; y++) {
            for (uint32_t z = 0; z < m_dim.z; z++) {
                MCVertex& v = m_vertices[Index3D(x, y, z)];
                v.position = grid.WorldPosition({x, y, z});
                v.insideShape = (m_density(v.position) > 0);
            }
        }
    }
}

void MCChunk::LabelEdges() 
{
    m_edges = new MCEdge[3*m_dim.x*m_dim.y*m_dim.z];

    for (uint32_t x = 0; x < m_dim.x; x++) {
        for (uint32_t y = 0; y < m_dim.y; y++) {
            for (uint32_t z = 0; z < m_dim.z; z++) {
                // x->x+1
                if (x < m_dim.x-1) {
                    CalculateEdgeData(Index3D(x, y, z), Index3D(x+1, y, z), 0);
                }
                // y->y+1
                if (y < m_dim.y-1) {
                    CalculateEdgeData(Index3D(x, y, z), Index3D(x, y+1, z), 1);
                }
                // z->z+1
                if (z < m_dim.z-1) {
                    CalculateEdgeData(Index3D(x, y, z), Index3D(x, y, z+1), 2);
                }
            }
        }
    }   
}

void MCChunk::CalculateEdgeData(uint32_t v1, uint32_t v2, uint32_t direction) 
{   
    MCEdge& edge = m_edges[3*v1+direction];

    // does the edge have a crossing ?
    edge.hasCrossing = (m_vertices[v1].insideShape != m_vertices[v2].insideShape);

    // if yes, calculate the crossing position
    if (edge.hasCrossing) {
        glm::vec3 start = m_vertices[v1].position;
        glm::vec3 end = m_vertices[v2].position;
        // binary search
        for (int i = 0; i < BIN_SEARCH_ITERATIONS; i++) {
            glm::vec3 middle = (start + end) / 2.0f;
            bool middleInside = m_density(middle) > 0;

            if (middleInside == m_vertices[v1].insideShape) {
                start = middle;
            }
            else {
                end = middle;
            }
        }
        glm::vec3 isoVertexPos = (start + end) / 2.0f;
        edge.isoVertexIdx = m_mesh->AddVertex(isoVertexPos, {1, 0, 0});
    }
}


void MCChunk::Triangulate()
{
    MCCell cell;
    for (uint32_t x = 0; x < m_dim.x-1; x++) {
        for (uint32_t y = 0; y < m_dim.y-1; y++) {
            for (uint32_t z = 0; z < m_dim.z-1; z++) {
                cell.x = x;
                cell.y = y;
                cell.z = z;
                CalculateCellEdges(cell);
                uint8_t hash = HashCell(cell);
                GenTriangles(mc_triangle_table[hash], cell);
            }
        }
    }
}

uint8_t MCChunk::HashCell(const MCCell& cell) 
{
    uint8_t hash = 0;
    for (uint32_t i = 0; i < 8; i++) {
        uint32_t x = cell.x + mc_vertex_delta[i][0];
        uint32_t y = cell.y + mc_vertex_delta[i][1];
        uint32_t z = cell.z + mc_vertex_delta[i][2];
        
        if (m_vertices[Index3D(x, y, z)].insideShape) {
            hash |= (1 << i);
        }
    }    
    return hash;
}

void MCChunk::CalculateCellEdges(MCCell& cell) 
{
    for (uint32_t i = 0; i < 12; i++) {
        uint32_t x = cell.x + mc_edge_delta[i][0];
        uint32_t y = cell.y + mc_edge_delta[i][1];
        uint32_t z = cell.z + mc_edge_delta[i][2];

        cell.edgesIndices[i] = 3*Index3D(x, y, z) + mc_edge_direction[i];
    }
}

void MCChunk::GenTriangles(const int triangles[16], const MCCell& cell) 
{
    uint32_t i = 0;
    while (i < 16) {
        if (triangles[i] == -1) {
            break;
        }
        int e1Idx = triangles[i++];
        int e2Idx = triangles[i++];
        int e3Idx = triangles[i++];

        const MCEdge& e1 = m_edges[cell.edgesIndices[e1Idx]];
        const MCEdge& e2 = m_edges[cell.edgesIndices[e2Idx]];
        const MCEdge& e3 = m_edges[cell.edgesIndices[e3Idx]];
        
        m_mesh->AddTriangle(e1.isoVertexIdx, e2.isoVertexIdx, e3.isoVertexIdx);
    }
}