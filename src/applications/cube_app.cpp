#include "applications/cube_app.h"
#include <glm/glm.hpp>
#include "third_party/imgui/imgui.h"
#include "third_party/imgui/imgui_impl_glfw.h"
#include "third_party/imgui/imgui_impl_opengl3.h"
#include <glm/gtc/matrix_transform.hpp>
#include "algorithms/cube_grid.h"
#include "rendering/gl_errors.h"



static float Circle(glm::vec3 pos)
{
    if (pos.z < 0) {
        return -1;
    }
    return 5 - glm::length(pos);
}

CubeApp::CubeApp()
{
    m_shader = new Shader("../shaders/Basic.shader");
    m_camera = new Camera({0, 0, 5.0f}, 10.0f, 1000.0f);

    // Create the marching cubes chunk.
    const auto& grid = CubeGrid(
        {20, 20, 20},
        {-10, -10, -10},
        {10, 10, 10});
    m_mcChunk = new MCChunk(grid, Circle);

    m_renderer->SetBackgroundColor({0.1, 0.1, 0.1, 1});
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
    if (m_inputMgr->m_rShiftKey == KeyState::PRESSED) {
        m_camera->Move({0.0, 1.0, 0.0});
    }
    if (m_inputMgr->m_rCtrlKey == KeyState::PRESSED) {
        m_camera->Move({0.0, -1.0, 0.0});
    }

    // rotate camera
    if (m_inputMgr->m_leftMouse == KeyState::PRESSED) {
        if (!m_rotateCamera) {
            m_inputMgr->DisableCursor();
            m_prevCursorPos = m_inputMgr->CursorPosition();
        }
        m_rotateCamera = true;
    }
    if (m_inputMgr->m_leftMouse == KeyState::RELEASED) {
        if (m_rotateCamera) {
            m_inputMgr->ShowCursor();
        }
        m_rotateCamera = false;
    }
    if (m_rotateCamera) {
        glm::vec2 cursorPos = m_inputMgr->CursorPosition();
        m_camera->RotateHorizontal(cursorPos.x - m_prevCursorPos.x);
        m_camera->RotateVertical(cursorPos.y - m_prevCursorPos.y);
        m_prevCursorPos = cursorPos;
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
        0.1f,
        10.0f);
    m_shader->Bind();
    m_shader->SetUniformMat4f("u_MVP", proj);
    const Mesh& mesh = m_mcChunk->GetMesh(); 

    // mesh
    //GLCall(glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ));
    //m_shader->SetUniform3f("u_color", 0.8, 0.8, 0);
    //m_renderer->Draw(mesh.GetVertexArray(), mesh.GetIndexBuffer(), *m_shader);
    
    // wireframe
    GLCall(glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ));
    m_shader->SetUniform3f("u_color", 1, 0, 1);
    m_renderer->Draw(mesh.GetVertexArray(), mesh.GetIndexBuffer(), *m_shader);
}