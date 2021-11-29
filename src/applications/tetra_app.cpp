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
    m_defaultShader = new Shader("../shaders/Default.shader");
    m_wireframeShader = new Shader("../shaders/Wireframe.shader");

    m_camera = new Camera({0, 0, 15.0f}, 50.0f, 1000.0f);
    m_drawOutline = false;
    m_outlineColor = {0.9, 0.3, 0.3};
    m_meshColor = {1, 1, 0};

    // Create the tetra hierarchy.
    uint32_t maxLevel = 10;
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
    delete m_defaultShader;
    delete m_wireframeShader;
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
    if (m_inputMgr->m_rightMouse == KeyState::PRESSED) {
        if (!m_rotateCamera) {
            m_inputMgr->DisableCursor();
            m_prevCursorPos = m_inputMgr->CursorPosition();
        }
        m_rotateCamera = true;
    }
    if (m_inputMgr->m_rightMouse == KeyState::RELEASED) {
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

    m_camera->UpdateMatrix(
        45.0f, 
        WINDOW_PIXEL_WIDTH / (float)WINDOW_PIXEL_HEIGHT,
        0.1f,
        1000.0f); 
}

void TetraApp::DrawImGui() 
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
        ImGui::Begin("Application info"); 
        ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate); 
        ImGui::ColorEdit3("Mesh color", (float*)&m_meshColor);
        ImGui::Checkbox("Draw outline", &m_drawOutline);
        if (m_drawOutline) {
            ImGui::ColorEdit3("Outline color", (float*)&m_outlineColor);
        }

        ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());   
}

void TetraApp::DrawOutline()
{
    m_wireframeShader->Bind();
    m_wireframeShader->SetUniformMat4f("u_worldToView", m_camera->WorldToViewMatrix());
    m_wireframeShader->SetUniformMat4f("u_viewToScreen", m_camera->ViewToScreenMatrix()); 
    
    const Mesh& outline = m_hierarchy->GetOutlineMesh();
   
    // wireframe outline
    GLCall(glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ));
    const glm::vec3& c = m_outlineColor;
    m_wireframeShader->SetUniform3f("u_color", c.x, c.y, c.z); 
    m_renderer->Draw(outline.GetVertexArray(), outline.GetIndexBuffer(), *m_wireframeShader);
}

void TetraApp::DrawMesh() 
{
    m_defaultShader->Bind();
    m_defaultShader->SetUniformMat4f("u_worldToView", m_camera->WorldToViewMatrix());
    m_defaultShader->SetUniformMat4f("u_viewToScreen", m_camera->ViewToScreenMatrix()); 
    m_defaultShader->SetUniform3f("u_lightDirection", -1, -1, 0);
    const glm::vec3& c = m_meshColor;
    m_defaultShader->SetUniform3f("u_color", c.x, c.y, c.z);
    GLCall(glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ));
    
    const Tetra* leaf = m_hierarchy->GetFirstLeafTetra();
    while (leaf) {
        m_renderer->Draw(leaf->mesh->GetVertexArray(), leaf->mesh->GetIndexBuffer(), *m_defaultShader);
        leaf = leaf->nextLeaf;
    }
}

void TetraApp::Render()
{
    if (m_drawOutline) {
        DrawOutline();
    }
    DrawMesh();
    DrawImGui();
}
