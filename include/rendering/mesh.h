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
    uint32_t AddVertex(const glm::vec3& position, const glm::vec3& color);
    void AddTriangle(uint32_t i1, uint32_t i2, uint32_t i3);
    
    void Build();

    const VertexArray& GetVertexArray() const;
    const IndexBuffer& GetIndexBuffer() const;
private:
    uint32_t m_vertexCount;
    // this is the buffer that will go in to the VertexBuffer
    std::vector<float> m_buffer;
    // the list of indices
    std::vector<IndexBuffer::index_t> m_indices;
    
    // these are built only when Build() is called
    VertexArray* m_va;
    IndexBuffer* m_ib;
    VertexBuffer* m_vb;

    void LayoutVector(const glm::vec3& v);
};
