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
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 color;
    };

    Mesh();
    ~Mesh();
    
    // The geometry is stored in CPU memory.
    // Returns the index of the added vertex.
    uint32_t AddVertex(const Vertex& v);
    void AddTriangle(uint32_t i1, uint32_t i2, uint32_t i3);
    void AddMesh(const Mesh& mesh);
    void Clear();
    
    // We have to reserve the right amount of GPU memory
    // before uploading.
    // mode can be GL_STATIC_DRAW or GL_DYNAMIC_DRAW.
    void AllocateGPUBuffers(GLenum mode, uint32_t vertexCount, uint32_t indexCount);
    void UploadGPUBuffers();

    uint32_t GetVertexCount() const;
    uint32_t GetTriangleCount() const;

    const VertexArray& GetVertexArray() const;
    const IndexBuffer& GetIndexBuffer() const;
    const Vertex* GetVertices() const;
    const uint32_t* GetIndices() const;
private:
    // this is the buffer that will go in to the VertexBuffer
    // vertexSize and vertexCapacity are the number of bytes.
    size_t m_vertexSize;
    size_t m_vertexCapacity;
    Vertex* m_vertices;
    // the list of indices.
    // indexSize and indexCapacity are the number of bytes.
    size_t m_indexSize;
    size_t m_indexCapacity;
    uint32_t* m_indices;
    
    VertexArray* m_va;
    IndexBuffer* m_ib;
    VertexBuffer* m_vb;

    void GrowVertexCapacity(size_t newCapacity);
    void GrowIndexCapacity(size_t newCapacity);
};
