#pragma once
#include <GL/glew.h> // include GLEW before anything else 
#include <GLFW/glfw3.h>
#include "scenes/triangle_scene.h"
#include "renderer.h"
#include "input_mgr.h"


class Application
{
private:
    GLFWwindow* m_window;
    TriangleScene* m_scene;
    Renderer* m_renderer;

    void createWindow();
    void initGlew();
    void setupInput();
    void createScene();
    void moveCamera();
public:
    InputManager* m_input_mgr;

    const static int windowPixelWidth = 1200;
    const static int windowPixelHeight = 900;
    Application();
    void mainLoop();
    void cleanup();
};