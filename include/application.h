#pragma once
#include <GL/glew.h> // include GLEW before anything else 
#include <GLFW/glfw3.h>
#include "scene.h"
#include "renderer.h"

class Application
{
private:
    GLFWwindow* m_window;
    Scene* m_scene;
    Renderer* m_renderer;

    void createWindow();
    void initGlew();
    void createScene();
public:
    const static int windowPixelWidth = 1200;
    const static int windowPixelHeight = 900;
    Application();
    void mainLoop();
    void cleanup();
    void keyCallback(int key, int action);
};