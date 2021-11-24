#include "rendering/mesh.h"


Mesh::Mesh() : m_vertexCount(0), m_builtMesh(false)
{

}


Mesh::~Mesh()
{
    if (m_builtMesh) {
        delete m_ib;
        delete m_vb;
        delete m_va;
    }
}

void Mesh::LayoutVector(const glm::vec3& v)
{
    m_buffer.push_back(v.x);
    m_buffer.push_back(v.y);
    m_buffer.push_back(v.z);
}

uint32_t Mesh::AddVertex(const glm::vec3& position, const glm::vec3& color) 
{
    LayoutVector(position);
    LayoutVector(color);
    m_vertexCount++;
    return m_vertexCount-1;
}

void Mesh::AddTriangle(uint32_t i1, uint32_t i2, uint32_t i3) 
{
    m_indices.push_back(i1);
    m_indices.push_back(i2);
    m_indices.push_back(i3);
}

void Mesh::Build() 
{
    VertexBufferLayout layout;
    layout.Push<float>(3); // vertex position
    layout.Push<float>(3); // vertex color

    m_ib = new IndexBuffer(m_indices.data(), m_indices.size());

    printf("buffer count = %d, layout stride = %d\n", m_buffer.size(), layout.GetStride());
    m_vb = new VertexBuffer(m_buffer.data(), m_buffer.size() * sizeof(float));
    m_va = new VertexArray();
    m_va->AddBuffer(*m_vb, layout);
    
    m_builtMesh = true;
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