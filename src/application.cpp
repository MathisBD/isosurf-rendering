#include "application.h"
#include <stdio.h>
#include "shader.h"
#include "timer.h"


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
}

void Application::initGlew()
{
    glewExperimental = GL_TRUE;
    glewInit();
}

void Application::setupInput()
{
    m_input_mgr = new InputManager();
    m_input_mgr->setupCallbacks(m_window);
}

void Application::createScene()
{
    glm::vec3 red = glm::vec3(1.0, 0.0, 0.0);
    glm::vec3 blue = glm::vec3(0.0, 0.0, 1.0);
    glm::vec3 green = glm::vec3(0.0, 1.0, 0.0);

    m_scene = new Scene();
    m_scene->m_camera.m_viewHeight = 10.0f;
    m_scene->m_camera.m_translationSpeed = 5.0f;
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
    setupInput();
    createScene();
    m_renderer = new Renderer();
    m_renderer->assignScene(m_scene);
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

void Application::moveCamera()
{
    if (m_input_mgr->m_leftKey == KeyState::PRESSED) {
        m_scene->translateCamera(glm::vec2(-1.0, 0.0));
    }
    if (m_input_mgr->m_rightKey == KeyState::PRESSED) {
        m_scene->translateCamera(glm::vec2(1.0, 0.0));
    }
    if (m_input_mgr->m_downKey == KeyState::PRESSED) {
        m_scene->translateCamera(glm::vec2(0.0, -1.0));
    }
    if (m_input_mgr->m_upKey == KeyState::PRESSED) {
        m_scene->translateCamera(glm::vec2(0.0, 1.0));
    }
}

void Application::mainLoop()
{
    // GLFW event loop
    while(!glfwWindowShouldClose(m_window))
    {   
        Timer::updateTime();
        moveCamera();
        m_renderer->renderTo(m_window);
        glfwPollEvents();
    }   
}



void Application::cleanup()
{
    glfwTerminate();
}