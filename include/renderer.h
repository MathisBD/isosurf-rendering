#pragma once
#include "scene.h"
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
    Scene* m_currentScene;
public:
    Renderer();
    void assignScene(Scene* scene);
    void renderTo(GLFWwindow* window);
};