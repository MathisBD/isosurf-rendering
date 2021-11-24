#include "rendering/renderer.h"
#include "rendering/vertex_buffer.h"
#include "rendering/vertex_array.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "rendering/gl_errors.h"


Renderer::Renderer() 
{
    GLCall(glEnable(GL_DEPTH_TEST));
}

void Renderer::Clear() const 
{    
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::SetBackgroundColor(const glm::vec4& color) 
{
    GLCall(glClearColor(color.r, color.g, color.b, color.a))  ;  
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const 
{
    shader.Bind();
    va.Bind();
    ib.Bind();

    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), IndexBuffer::s_glIndexType, nullptr));
}