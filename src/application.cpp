#include "application.h"
#include <stdio.h>
#include "shader.h"

bool isArrowKey(int glfwKey)
{
    switch (glfwKey) {
    case GLFW_KEY_LEFT:
    case GLFW_KEY_RIGHT:
    case GLFW_KEY_UP:
    case GLFW_KEY_DOWN:
        return true;
    default:
        return false;
    }
}

glm::vec2 arrowKeyDirection(int glfwKey)
{
    switch (glfwKey) {
    case GLFW_KEY_LEFT: return glm::vec2(-1.0, 0.0);
    case GLFW_KEY_RIGHT: return glm::vec2(1.0, 0.0);
    case GLFW_KEY_UP: return glm::vec2(0.0, 1.0);
    case GLFW_KEY_DOWN: return glm::vec2(0.0, -1.0);
    default: return glm::vec2(0.0, 0.0);
    }
}

void proxyKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    app->keyCallback(key, action);
}

void Application::keyCallback(int key, int action)
{ 
    if (action == GLFW_REPEAT) {
        if (isArrowKey(key)) {
            glm::vec2 dir = arrowKeyDirection(key);
            m_scene->translateCamera(dir);
        }
    }
}

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
    glfwSetWindowUserPointer(m_window, this);
    glfwMakeContextCurrent(m_window);

    glfwSetKeyCallback(m_window, proxyKeyCallback);
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