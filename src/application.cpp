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

// a simple circle
float density(glm::vec2 pos)
{
    return 3.0f - glm::length(pos);
}

GridScene* sampleGridScene()
{
    auto scene = new GridScene(1000, 1000, -5.0, 5.0, -5.0, 5.0);
    for (size_t x = 0; x < scene->m_subdivs_x; x++) {
        for (size_t y = 0; y < scene->m_subdivs_y; y++) {
            glm::vec2 pos = scene->worldPosition(x, y);
            scene->m_grid[x][y] = density(pos);
        }
    }
    return scene;
}

void Application::createScene()
{
    glm::vec3 red = glm::vec3(1.0, 0.0, 0.0);
    glm::vec3 blue = glm::vec3(0.0, 0.0, 1.0);
    glm::vec3 green = glm::vec3(0.0, 1.0, 0.0);
    glm::vec3 white = glm::vec3(1.0, 1.0, 1.0);

    auto grid = sampleGridScene(); 

    m_scene = new TriangleScene();
    m_scene->m_camera.m_viewHeight = 10.0f;
    m_scene->m_camera.m_translationSpeed = 5.0f;

    // draw the grid lines
    float lineWidth = 0.02f; 
    for (size_t x = 0; x < grid->m_subdivs_x; x++) {
        m_scene->addLine(
            glm::vec3(grid->worldPosition(x, 0), 0.0f),
            glm::vec3(grid->worldPosition(x, grid->m_subdivs_y-1), 0.0f),
            white,
            lineWidth);
    }
    for (size_t y = 0; y < grid->m_subdivs_y; y++) {
        m_scene->addLine(
            glm::vec3(grid->worldPosition(0, y), 0.0f),
            glm::vec3(grid->worldPosition(grid->m_subdivs_x-1, y), 0.0f),
            white,
            lineWidth);
    }
    m_scene->addTriangle(
        glm::vec3(0.0, 0.0, 0.0),
        glm::vec3(0.0, 1.0, 0.0),
        glm::vec3(1.0, 0.0, 0.0), green);
    m_scene->addTriangle(
        glm::vec3(1.0, 0.0, 0.0),
        glm::vec3(0.0, 1.0, 0.0),
        glm::vec3(1.0, 1.0, 0.0), green);
    /*m_scene->addLine(
        glm::vec3(0.0, 1.0, 0.0),
        glm::vec3(-3.0, 2.0, 0.0),
        white,
        0.01);*/
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