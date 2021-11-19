#pragma once
#include <GL/glew.h> // include GLEW before anything else 
#include <GLFW/glfw3.h>
#include "triangle_scene.h"
#include "renderer.h"
#include "input_mgr.h"


class Application
{
protected:
    GLFWwindow* m_window;
    TriangleScene* m_scene;
    Renderer* m_renderer;

    void createWindow();
    void initGlew();
    void setupInput();
    void moveCamera();
    virtual void createScene() = 0;
public:
    InputManager* m_input_mgr;

    const static int windowPixelWidth = 1200;
    const static int windowPixelHeight = 900;
    Application();
    void initialize();
    void mainLoop();
    void cleanup();
};