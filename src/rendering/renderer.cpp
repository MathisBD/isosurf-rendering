#include "rendering/renderer.h"
#include "rendering/vertex_buffer.h"
#include "rendering/vertex_array.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "rendering/gl_errors.h"


Renderer::Renderer() 
{

}

void Renderer::Clear() const 
{    
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const Shader& shader, unsigned int triangleCount) const 
{
    shader.Bind();
    va.Bind();

    GLCall(glDrawArrays(GL_TRIANGLES, 0, triangleCount));
}