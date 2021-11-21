#include "rendering/vertex_buffer.h"
#include <GL/glew.h>


VertexBuffer::VertexBuffer(void* data, size_t size) 
{
    // create the Vertex Buffer Object
    glGenBuffers(1, &m_rendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer() 
{
    glDeleteBuffers(1, &m_rendererID);
}

void VertexBuffer::Bind() const 
{
    glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
}

void VertexBuffer::Unbind() const
{

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}