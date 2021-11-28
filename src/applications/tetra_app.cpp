#include "applications/tetra_app.h"
#include <glm/glm.hpp>
#include "third_party/imgui/imgui.h"
#include "third_party/imgui/imgui_impl_glfw.h"
#include "third_party/imgui/imgui_impl_opengl3.h"
#include <glm/gtc/matrix_transform.hpp>
#include "algorithms/cube_grid.h"
#include "rendering/gl_errors.h"
#include "algorithms/tetra_hierarchy.h"
#include <chrono>
#include <stdio.h>


static float Circle(glm::vec3 pos)
{
    return 100.0f - glm::distance(pos, {0, 0, -100});
}

TetraApp::TetraApp()
{
    m_shader = new Shader("../shaders/Basic.shader");
    m_camera = new Camera({0, 0, 15.0f}, 50.0f, 1000.0f);

    // Create the tetra hierarchy.
    uint32_t maxLevel = 15;
    uint32_t mcChunkDim = 4;
    uint32_t maxCoord = TetraHierarchy::MaxCoord(maxLevel);
    CubeGrid grid = CubeGrid(
        maxCoord+1,
        {0.0f, 0.0f, -100.0f},
        100.0f);
    m_hierarchy = new TetraHierarchy(grid, Circle, maxLevel, mcChunkDim);

    auto start = std::chrono::high_resolution_clock::now();
    m_hierarchy->SplitAll({0, 0, 0}, 0.5);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    printf("Time to split the hierarchy : %ldms\n", time.count());
   
    m_renderer->SetBackgroundColor({0.1, 0.1, 0.1, 1});
}

TetraApp::~TetraApp() 
{
    delete m_camera;
    delete m_shader;
    delete m_hierarchy;
}

void TetraApp::Update()
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
        m_camera->RotateHorizontal(-(cursorPos.x - m_prevCursorPos.x));
        m_camera->RotateVertical(-(cursorPos.y - m_prevCursorPos.y));
        m_prevCursorPos = cursorPos;
    }
}

void TetraApp::Render()
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
        1000.0f);
    m_shader->Bind();
    m_shader->SetUniformMat4f("u_camera", proj);
    m_shader->SetUniform3f("u_lightDirection", 1.0f, 0.0f, 0.0f);
    const Mesh& mesh = m_hierarchy->GetOutlineMesh();
   
    // wireframe outline
    GLCall(glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ));
    m_shader->SetUniform3f("u_color", 1, 0, 1);
    m_renderer->Draw(mesh.GetVertexArray(), mesh.GetIndexBuffer(), *m_shader);
    
    // actual mesh
    GLCall(glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ));
    m_shader->SetUniform3f("u_color", 1, 1, 0);
    const Tetra* leaf = m_hierarchy->GetFirstLeafTetra();
    while (leaf) {
        m_renderer->Draw(leaf->mesh->GetVertexArray(), leaf->mesh->GetIndexBuffer(), *m_shader);
        leaf = leaf->nextLeaf;
    }
}
