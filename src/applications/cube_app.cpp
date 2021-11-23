#include "applications/cube_app.h"
#include <glm/glm.hpp>
#include "third_party/imgui/imgui.h"
#include "third_party/imgui/imgui_impl_glfw.h"
#include "third_party/imgui/imgui_impl_opengl3.h"
#include <glm/gtc/matrix_transform.hpp>


CubeApp::CubeApp()
{
    m_shader = new Shader("../shaders/Basic.shader");
    m_camera = new Camera({0, 0, 5.0f}, 10.0f);
   
    glm::vec3 red(1, 0, 0);
    m_mesh = new Mesh();

    /*m_mesh->AddVertex({0, 0, 0}, red); 
    m_mesh->AddVertex({0, 0, 1}, red); 
    m_mesh->AddVertex({0, 1, 0}, red); 
    m_mesh->AddVertex({0, 1, 1}, red); 
    m_mesh->AddVertex({1, 0, 0}, red); 
    m_mesh->AddVertex({1, 0, 1}, red); 
    m_mesh->AddVertex({1, 1, 0}, red); 
    m_mesh->AddVertex({1, 1, 1}, red); */
    
    m_mesh->AddVertex({-0.5, 0.0, 0.0}, {1.0, 0.0, 0.0});
    m_mesh->AddVertex({0.5, 0.0, 0.0}, {0.0, 1.0, 0.0});
    m_mesh->AddVertex({0.0, 0.5, 0.0}, {0.0, 0.0, 1.0});
    m_mesh->AddTriangle(0, 1, 2);
    m_mesh->Build();
}

CubeApp::~CubeApp() 
{
    delete m_camera;
    delete m_mesh;
    delete m_shader;
}



void CubeApp::Update()
{
    // translate camera
    if (m_inputMgr->m_leftKey == KeyState::PRESSED) {
        m_camera->Move({1.0, 0.0, 0.0});
    }
    if (m_inputMgr->m_rightKey == KeyState::PRESSED) {
        m_camera->Move({-1.0, 0.0, 0.0});
    }
    if (m_inputMgr->m_downKey == KeyState::PRESSED) {
        m_camera->Move({0.0, 0.0, -1.0});
    }
    if (m_inputMgr->m_upKey == KeyState::PRESSED) {
        m_camera->Move({0.0, 0.0, 1.0});
    }

    // rotate camera
    if (m_inputMgr->m_leftMouse == KeyState::PRESSED) {
        m_inputMgr->DisableCursor();
    }
    else {
        m_inputMgr->ShowCursor();
    }
}

void CubeApp::Render()
{
    // ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
        ImGui::Begin("Application info"); 
        ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate); 
        
        glm::vec2 cursorPos = m_inputMgr->CursorPosition();
        ImGui::Text("Cursor position : (%.2f, %.2f)", cursorPos.x, cursorPos.y);

        ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Renderer frame
    glm::mat4 proj = m_camera->ProjectionMatrix(
        45.0f, 
        WINDOW_PIXEL_WIDTH / (float)WINDOW_PIXEL_HEIGHT,
        0.1f);
    m_shader->Bind();
    m_shader->SetUniformMat4f("u_MVP", proj);
    m_renderer->Draw(m_mesh->GetVertexArray(), m_mesh->GetIndexBuffer(), *m_shader);
}