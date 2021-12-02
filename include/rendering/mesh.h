#pragma once
#include <vector>
#include <array>
#include <glm/glm.hpp>
#include "rendering/vertex_array.h"
#include "rendering/vertex_buffer.h"
#include "rendering/vertex_buffer_layout.h"
#include "rendering/index_buffer.h"


class Mesh
{
public:
    Mesh();
    ~Mesh();
    
    // Returns the index of the added vertex.
    uint32_t AddVertex(const glm::vec3& position);
    void AddTriangle(uint32_t i1, uint32_t i2, uint32_t i3);
    
    void Build();

    uint32_t GetVertexCount() const;
    uint32_t GetTriangleCount() const;

    const VertexArray& GetVertexArray() const;
    const IndexBuffer& GetIndexBuffer() const;
private:
    // this is the buffer that will go in to the VertexBuffer
    // vertexCount and vertexCapacity are actually 6 times 
    // the number of vertices, they count the number of floats.
    size_t m_vertexCount;
    size_t m_vertexCapacity;
    float* m_vertices;
    // the list of indices
    size_t m_indexCount;
    size_t m_indexCapacity;
    uint32_t* m_indices;
    
    VertexArray* m_va;
    IndexBuffer* m_ib;
    VertexBuffer* m_vb;

    void LayoutVector(const glm::vec3& v);
};
