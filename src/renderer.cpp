#include "renderer.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>


Renderer::Renderer()
{
    // create the Vertex Array Object
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // create the Vertex Buffer Object
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    
    // compile shaders
    m_vertexShader = new Shader(GL_VERTEX_SHADER, "../shaders/vertex.glsl");
    m_vertexShader->compile();
    m_fragmentShader = new Shader(GL_FRAGMENT_SHADER, "../shaders/fragment.glsl");
    m_fragmentShader->compile();

    // link the shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, m_vertexShader->glid);
    glAttachShader(shaderProgram, m_fragmentShader->glid);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
    
    // get the shader attributes
    GLint positionHandle = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(positionHandle);
    glVertexAttribPointer(positionHandle, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(float), 0);
    
    GLint colorHandle = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colorHandle);
    glVertexAttribPointer(colorHandle, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(float), (void*)(3 * sizeof(float)));
}

void layoutVector(const glm::vec3& vect, float* array)
{
    array[0] = vect.x;
    array[1] = vect.y;
    array[2] = vect.z;
}

void Renderer::assignScene(Scene* scene)
{
    m_currentScene = scene;

    // layout the geometry in a flat array
    size_t count = m_currentScene->m_triangles.size();
    float* vertices = new float[6*count];
    for (size_t i = 0; i < count; i++) {
        Vertex vert = m_currentScene->m_triangles[i];
        layoutVector(vert.m_position, vertices + 6*i);
        layoutVector(vert.m_color, vertices + 6*i + 3);
    }
    // load the geometry on the GPU
    glBufferData(GL_ARRAY_BUFFER, 6*count*sizeof(float), vertices, GL_STATIC_DRAW);
}

void Renderer::renderTo(GLFWwindow* window)
{
    glDrawArrays(GL_TRIANGLES, 0, m_currentScene->m_triangles.size());
    glfwSwapBuffers(window);
}