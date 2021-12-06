#include "rendering/index_buffer.h"
#include "rendering/gl_errors.h"
#include <stdio.h>

IndexBuffer::IndexBuffer(GLenum mode, const void* data, size_t size) : 
    m_size(size) 
{
    GLCall(glGenBuffers(1, &m_rendererID));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, mode)); 
}

IndexBuffer::~IndexBuffer() 
{
    GLCall(glDeleteBuffers(1, &m_rendererID));
}

void IndexBuffer::UploadData(const void* data, size_t size) 
{
    assert(size <= m_size);
    Bind();
    GLCall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, data));    
}

void IndexBuffer::Bind() const 
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID));    
}

void IndexBuffer::Unbind() const 
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));    
}

size_t IndexBuffer::GetSize() const
{
    return m_size;    
}