#include "rendering/vertex_array.h"
#include <GL/glew.h>
#include "rendering/gl_errors.h"


VertexArray::VertexArray() 
{
    GLCall(glGenVertexArrays(1, &m_rendererID));
}

VertexArray::~VertexArray() 
{
    GLCall(glDeleteVertexArrays(1, &m_rendererID));    
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) 
{
    Bind();
    vb.Bind();

    const auto& elements = layout.GetElements();
    size_t offset = 0;
    for (size_t i = 0; i < elements.size(); i++) {
        const auto& e = elements[i];
        GLCall(glEnableVertexAttribArray(i));
        GLCall(glVertexAttribPointer(i, e.count, e.glType, GL_FALSE, 
            layout.GetStride(), (void*)offset));
        offset += e.count * VertexBufferLayout::SizeOfGlType(e.glType);
    }
}

void VertexArray::Bind() const 
{
    glBindVertexArray(m_rendererID);
}

void VertexArray::Unbind() const 
{
    glBindVertexArray(0);    
}