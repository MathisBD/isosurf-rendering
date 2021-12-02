#pragma once
#include <GL/glew.h> // include GLEW before anything else 
#include <GLFW/glfw3.h>
#include "rendering/renderer.h"
#include "input_mgr.h"


class Application
{
protected:
    GLFWwindow* m_window = nullptr;
    Renderer* m_renderer = nullptr;
    InputManager* m_inputMgr = nullptr;
    
    void CreateWindow();
    void InitGlew();
    void SetupInput();
    void InitImGui();

    // child classes should override these methos
    virtual void Update() = 0;
    virtual void Render() = 0;
public:
    const static int WINDOW_PIXEL_WIDTH = 1900;
    const static int WINDOW_PIXEL_HEIGHT = 1000;

    Application();
    ~Application();
    void MainLoop();

    InputManager& GetInputManager();
};