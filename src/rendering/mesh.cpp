#include "rendering/mesh.h"
#include <string.h>


Mesh::Mesh()
{
    m_vertexSize = m_indexSize = 0;
    m_vertexCapacity = 8 * sizeof(Vertex);
    m_indexCapacity = 8 * sizeof(uint32_t);
    m_indices = (uint32_t*)malloc(m_indexCapacity);
    m_vertices = (Vertex*)malloc(m_vertexCapacity);

    m_va = nullptr;
    m_ib = nullptr;
    m_vb = nullptr;
}


Mesh::~Mesh()
{
    free(m_indices);
    free(m_vertices);
    delete m_ib;
    delete m_vb;
    delete m_va;
}

uint32_t Mesh::AddVertex(const Vertex& v) 
{
    // make some space for the vertex
    GrowVertexCapacity(m_vertexSize + sizeof(Vertex));
    memcpy(m_vertices + (m_vertexSize / sizeof(Vertex)), &v, sizeof(Vertex));
    m_vertexSize += sizeof(Vertex);
    // return the index of the vertex we added.
    return (m_vertexSize / sizeof(Vertex)) - 1;
}

void Mesh::AddTriangle(uint32_t i1, uint32_t i2, uint32_t i3) 
{
    GrowIndexCapacity(m_indexSize + 3 * sizeof(uint32_t));
    m_indices[m_indexSize / sizeof(uint32_t)] = i1;
    m_indexSize += sizeof(uint32_t);
    m_indices[m_indexSize / sizeof(uint32_t)] = i2;
    m_indexSize += sizeof(uint32_t);
    m_indices[m_indexSize / sizeof(uint32_t)] = i3;
    m_indexSize += sizeof(uint32_t);
}

void Mesh::Clear()
{
    m_indexSize = m_vertexSize = 0;
}

void Mesh::AllocateGPUBuffers(GLenum mode, uint32_t vertexCount, uint32_t indexCount) 
{
    VertexBufferLayout layout;
    layout.Push<float>(3); // vertex position
    layout.Push<float>(3); // vertex color

    delete m_vb;
    m_vb = new VertexBuffer(mode, nullptr, vertexCount * sizeof(Vertex));
    delete m_ib;
    m_ib = new IndexBuffer(mode, nullptr, indexCount * sizeof(uint32_t));    
    delete m_va;
    m_va = new VertexArray();
    m_va->AddBuffer(*m_vb, layout);
}

void Mesh::UploadGPUBuffers() 
{
    assert(m_vb && m_ib && m_va);
    m_vb->UploadData(m_vertices, m_vertexSize);
    m_ib->UploadData(m_indices, m_indexSize);
}

void Mesh::AddMesh(const Mesh& other) 
{
    // copy the indices
    GrowIndexCapacity(m_indexSize + other.m_indexSize);
    for (size_t i = 0; i < (other.m_indexSize / sizeof(uint32_t)); i++) {
        m_indices[(m_indexSize / sizeof(uint32_t)) + i] = 
            (m_vertexSize / sizeof(Vertex)) + other.m_indices[i];
    }
    m_indexSize += other.m_indexSize;
    // copy the vertices
    GrowVertexCapacity(m_vertexSize + other.m_vertexSize);
    memcpy(
        m_vertices + (m_vertexSize / sizeof(Vertex)), 
        other.m_vertices, 
        other.m_vertexSize);
    m_vertexSize += other.m_vertexSize;
    
}

const IndexBuffer& Mesh::GetIndexBuffer() const
{
    assert(m_ib);
    return *m_ib;    
}

const VertexArray& Mesh::GetVertexArray() const
{
    assert(m_va);
    return *m_va;
}

const uint32_t* Mesh::GetIndices() const 
{
    return m_indices;    
}

const Mesh::Vertex* Mesh::GetVertices() const 
{
    return m_vertices;    
}

uint32_t Mesh::GetTriangleCount() const
{
    return m_indexSize / (3 * sizeof(uint32_t)); // there are 3 indices per triangle
}

uint32_t Mesh::GetVertexCount() const
{
    return m_vertexSize / sizeof(Vertex);
}

void Mesh::GrowIndexCapacity(size_t newCapacity) 
{
    while (newCapacity > m_indexCapacity) {
        m_indexCapacity *= 2;
    }
    m_indices = (uint32_t*)realloc(m_indices, m_indexCapacity);
}

void Mesh::GrowVertexCapacity(size_t newCapacity) 
{
    while (newCapacity > m_vertexCapacity) {
        m_vertexCapacity *= 2;
    }
    m_vertices = (Vertex*)realloc(m_vertices, m_vertexCapacity); 
}