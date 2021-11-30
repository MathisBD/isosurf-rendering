#include "rendering/mesh.h"


Mesh::Mesh()
{
    m_vertexCount = m_indexCount = 0;
    m_vertexCapacity = m_indexCapacity = 8;
    m_indices = (uint32_t*)malloc(m_indexCapacity * sizeof(uint32_t));
    m_vertices = (float*)malloc(m_vertexCapacity * sizeof(float));

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

inline void Mesh::LayoutVector(const glm::vec3& v)
{
    m_vertices[m_vertexCount++] = v.x;
    m_vertices[m_vertexCount++] = v.y;
    m_vertices[m_vertexCount++] = v.z;
}


uint32_t Mesh::AddVertex(const glm::vec3& position) 
{
    // make some space for the vertex
    if (m_vertexCount + 3 > m_vertexCapacity) {
        // since the initial capacity is greater than 3,
        // we can just double the capacity.
        m_vertexCapacity *= 2;
        m_vertices = (float*)realloc(m_vertices, m_vertexCapacity * sizeof(float));
    }
    LayoutVector(position);
    // return the index of the vertex we added.
    return (m_vertexCount / 3) - 1;
}

void Mesh::AddTriangle(uint32_t i1, uint32_t i2, uint32_t i3) 
{
    // make some space for the indices
    if (m_indexCount + 3 > m_indexCapacity) {
        // since the initial capacity is greater than 3,
        // we can just double the capacity.
        m_indexCapacity *= 2;
        m_indices = (uint32_t*)realloc(m_indices, m_indexCapacity * sizeof(uint32_t));
    }
    m_indices[m_indexCount++] = i1;
    m_indices[m_indexCount++] = i2;
    m_indices[m_indexCount++] = i3;
}

void Mesh::Build() 
{
    VertexBufferLayout layout;
    layout.Push<float>(3); // vertex position

    if (m_ib) { delete m_ib; }
    m_ib = new IndexBuffer(m_indices, m_indexCount);
    if (m_vb) { delete m_vb; }
    m_vb = new VertexBuffer(m_vertices, m_vertexCount * sizeof(float));
    if (m_va) { delete m_va; }
    m_va = new VertexArray();
    m_va->AddBuffer(*m_vb, layout);
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