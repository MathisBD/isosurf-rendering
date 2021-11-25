#include "applications/application.h"
#include <stdio.h>
#include "rendering/shader.h"
#include "timer.h"
#include <iostream>
#include "third_party/imgui/imgui.h"
#include "third_party/imgui/imgui_impl_glfw.h"
#include "third_party/imgui/imgui_impl_opengl3.h"


Application::Application()
{
    CreateWindow();
    InitGlew();
    InitImGui();
    SetupInput();
    m_renderer = new Renderer();
}

Application::~Application()
{
    delete m_renderer;
    delete m_inputMgr;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Application::CreateWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    m_window = glfwCreateWindow(
        Application::WINDOW_PIXEL_WIDTH, 
        Application::WINDOW_PIXEL_HEIGHT, 
        "OpenGL", nullptr, nullptr); // Windowed
    glfwSetWindowUserPointer(m_window, this);
    glfwMakeContextCurrent(m_window);
}

void Application::InitGlew()
{
    glewExperimental = GL_TRUE;
    glewInit();
}

void Application::SetupInput()
{
    m_inputMgr = new InputManager(m_window);
}

void Application::InitImGui()
{
    // imgui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // imgui style
    ImGui::StyleColorsDark();
    
    // imgui backend
    const char* glsl_version = "#version 130";
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}


void Application::MainLoop()
{   
    // GLFW event loop
    while(!glfwWindowShouldClose(m_window))
    {   
        Timer::UpdateTime();
        Update();
        
        m_renderer->Clear();
        Render();
        glfwSwapBuffers(m_window);
        
        glfwPollEvents();
    }   
}


InputManager& Application::GetInputManager() 
{
    return *m_inputMgr;    
}