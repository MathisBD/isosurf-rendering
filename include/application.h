#pragma once
#include <GL/glew.h> // include GLEW before anything else 
#include <GLFW/glfw3.h>
#include "triangle_scene.h"
#include "rendering/renderer.h"
#include "input_mgr.h"


class Application
{
protected:
    GLFWwindow* m_window;
    TriangleScene* m_scene;
    Renderer* m_renderer;
    InputManager* m_inputMgr;

    void CreateWindow();
    void InitGlew();
    void SetupInput();
    void MoveCamera();
    void InitImGui();
    virtual void CreateScene() = 0;
public:
    const static int WINDOW_PIXEL_WIDTH = 1200;
    const static int WINDOW_PIXEL_HEIGHT = 900;

    Application();
    
    void Initialize();
    void MainLoop();
    void Cleanup();

    InputManager& GetInputManager();
};