#include "application.h"
#include "scenes/grid_scene.h"
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


Application::Application()
{

}

void Application::initialize()
{
    createWindow();
    initGlew();
    setupInput();
    createScene();
    m_renderer = new Renderer();
    m_renderer->assignScene(m_scene);
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