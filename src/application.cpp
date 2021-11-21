#include "application.h"
#include <stdio.h>
#include "rendering/shader.h"
#include "timer.h"
#include <iostream>
#include "third_party/imgui/imgui.h"
#include "third_party/imgui/imgui_impl_glfw.h"
#include "third_party/imgui/imgui_impl_opengl3.h"


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
    m_inputMgr = new InputManager();
    m_inputMgr->SetupCallbacks(m_window);
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


Application::Application()
{

}

void Application::Initialize()
{
    CreateWindow();
    InitGlew();
    InitImGui();
    SetupInput();
    CreateScene();
    m_renderer = new Renderer();
}

void Application::MoveCamera()
{
    if (m_inputMgr->m_leftKey == KeyState::PRESSED) {
        m_scene->TranslateCamera(glm::vec2(-1.0, 0.0));
    }
    if (m_inputMgr->m_rightKey == KeyState::PRESSED) {
        m_scene->TranslateCamera(glm::vec2(1.0, 0.0));
    }
    if (m_inputMgr->m_downKey == KeyState::PRESSED) {
        m_scene->TranslateCamera(glm::vec2(0.0, -1.0));
    }
    if (m_inputMgr->m_upKey == KeyState::PRESSED) {
        m_scene->TranslateCamera(glm::vec2(0.0, 1.0));
    }
}

static void LayoutVector(const glm::vec3& vect, float* array)
{
    array[0] = vect.x;
    array[1] = vect.y;
    array[2] = vect.z;
}

void Application::MainLoop()
{
    // layout the geometry in a flat array
    size_t count = m_scene->GetTriangles().size();
    float* vertices = new float[6 * count];
    for (size_t i = 0; i < count; i++) {
        const Vertex& vert = m_scene->GetTriangles()[i];
        LayoutVector(vert.m_position, vertices + 6*i);
        LayoutVector(vert.m_color, vertices + 6*i + 3);
    }
    // vertex buffer
    VertexBuffer vb(vertices, 6 * count * sizeof(float));
    // vertex buffer layout
    VertexBufferLayout layout;
    layout.Push<float>(3); // vertex position 
    layout.Push<float>(3); // vertex color
    // vertex array
    VertexArray va;
    va.AddBuffer(vb, layout);
    // shader
    Shader shader("../shaders/Basic.shader");

    // GLFW event loop
    while(!glfwWindowShouldClose(m_window))
    {   
        Timer::UpdateTime();
        MoveCamera();
        m_renderer->Clear();

        // ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("Application info");                          // Create a window called "Hello, world!" and append into it.
            ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate); 
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Renderer frame
        const Camera& cam = m_scene->GetCamera();
        shader.Bind();
        shader.SetUniform2f(
            "u_cameraCenterPos", 
            cam.m_centerPos.x,
            cam.m_centerPos.y);
        shader.SetUniform2f(
            "u_cameraViewSize", 
            (cam.m_viewHeight * Application::WINDOW_PIXEL_WIDTH) / Application::WINDOW_PIXEL_HEIGHT,
            cam.m_viewHeight);
        m_renderer->Draw(va, shader, m_scene->GetTriangles().size());
        
        
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }   
}


void Application::Cleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_window);
    glfwTerminate();
}

InputManager& Application::GetInputManager() 
{
    return *m_inputMgr;    
}