#include "application.h"
#include <stdio.h>
#include "shader.h"


void Application::createWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    m_window = glfwCreateWindow(
        Application::windowPixelWidth, 
        Application::windowPixelHeight, 
        "OpenGL", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(m_window);
}

void Application::initGlew()
{
    glewExperimental = GL_TRUE;
    glewInit();
}

void Application::createScene()
{
    glm::vec3 red = glm::vec3(1.0, 0.0, 0.0);
    glm::vec3 blue = glm::vec3(0.0, 0.0, 1.0);
    glm::vec3 green = glm::vec3(0.0, 1.0, 0.0);

    m_scene = new Scene();
    m_scene->m_camera.m_viewHeight = 1.0f;
    m_scene->addTriangle(
        Vertex(glm::vec3(0.0, 0.5, 0.0), red),
        Vertex(glm::vec3(0.0, -0.5, 0.0), green),
        Vertex(glm::vec3(-0.5, 0.0, 0.0), green));
    m_scene->addTriangle(
        Vertex(glm::vec3(0.0, 0.5, 0.0), red),
        Vertex(glm::vec3(0.0, -0.5, 0.0), green),
        Vertex(glm::vec3(0.5, 0.0, 0.0), green));
}

Application::Application()
{
    createWindow();
    initGlew();
    createScene();
    m_renderer = new Renderer();
    m_renderer->assignScene(m_scene);
}

void Application::mainLoop()
{
    // GLFW event loop
    while(!glfwWindowShouldClose(m_window))
    {   
        m_renderer->renderTo(m_window);
        glfwPollEvents();
    }   
}

void Application::cleanup()
{
    glfwTerminate();
}