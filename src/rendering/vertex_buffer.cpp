#include "rendering/vertex_buffer.h"
#include <GL/glew.h>
#include "rendering/gl_errors.h"


VertexBuffer::VertexBuffer(GLenum mode, const void* data, size_t size) :
    m_size(size)
{
    // create the Vertex Buffer Object
    GLCall(glGenBuffers(1, &m_rendererID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_rendererID));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, mode));
}

VertexBuffer::~VertexBuffer() 
{
    GLCall(glDeleteBuffers(1, &m_rendererID));
}

void VertexBuffer::UploadData(const void* data, size_t size) 
{
    assert(size <= m_size);
    Bind();  
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
}

void VertexBuffer::Bind() const 
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_rendererID));
}

void VertexBuffer::Unbind() const
{

    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}