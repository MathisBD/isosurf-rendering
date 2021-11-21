#include "rendering/vertex_buffer_layout.h"
#include <GL/glew.h>
#include <assert.h>


VertexBufferLayout::VertexBufferLayout() : m_stride(0) 
{

}

unsigned int VertexBufferLayout::GetStride() const
{
    return m_stride;
}

const std::vector<VertexBufferLayout::LayoutElement>& VertexBufferLayout::GetElements() const
{
    return m_elements;
}

unsigned int VertexBufferLayout::SizeOfGlType(unsigned int glType) 
{
    switch (glType)
    {
    case GL_FLOAT: return 4;
    case GL_UNSIGNED_INT: return 4;
    }    
    assert(false);
    return 0; 
}

template<typename T>
void VertexBufferLayout::Push(unsigned int count) 
{
    assert(false);
}

template<>
void VertexBufferLayout::Push<float>(unsigned int count)
{
    m_elements.push_back({GL_FLOAT, count});
    m_stride += count * SizeOfGlType(GL_FLOAT);    
}

template<>
void VertexBufferLayout::Push<unsigned int>(unsigned int count)
{
    m_elements.push_back({GL_UNSIGNED_INT, count});
    m_stride += count * SizeOfGlType(GL_UNSIGNED_INT);    
}

