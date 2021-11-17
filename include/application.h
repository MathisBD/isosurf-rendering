#pragma once
#include <GL/glew.h> // include GLEW before anything else 
#include <GLFW/glfw3.h>
#include "scene.h"
#include "renderer.h"

class Application
{
private:
    const static int windowPixelWidth = 800;
    const static int windowPixelHeight = 600;
    GLFWwindow* m_window;
    Scene* m_scene;
    Renderer* m_renderer;

    void createWindow();
    void initGlew();
    void createScene();
public:
    Application();
    void mainLoop();
    void cleanup();
};