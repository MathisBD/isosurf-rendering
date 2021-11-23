#include "rendering/index_buffer.h"
#include "rendering/gl_errors.h"


IndexBuffer::IndexBuffer(const index_t* data, uint32_t count) : m_count(count) 
{
    GLCall(glGenBuffers(1, &m_rendererID));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(index_t), data, GL_STATIC_DRAW));    
}

IndexBuffer::~IndexBuffer() 
{
    GLCall(glDeleteBuffers(1, &m_rendererID));    
}

void IndexBuffer::Bind() const 
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID));    
}

void IndexBuffer::Unbind() const 
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));    
}

uint32_t IndexBuffer::GetCount() const
{
    return m_count;    
}