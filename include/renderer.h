#pragma once
#include "triangle_scene.h"
#include "shader.h"
#include <GLFW/glfw3.h>


class Renderer
{
private:
    GLuint m_vao; 
    GLuint m_vbo; 
    Shader* m_vertexShader;
    Shader* m_fragmentShader;
    GLint m_cameraPosHandle;
    GLint m_cameraSizeHandle;
    TriangleScene* m_triangleScene;
public:
    Renderer();
    void assignScene(TriangleScene* scene);
    void renderTo(GLFWwindow* window);
};