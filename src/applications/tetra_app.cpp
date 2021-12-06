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
#include <thread>


static float Sphere(glm::vec3 pos)
{
    return 100.0f - glm::distance(pos, {0, 0, -100});
}

static float Noise(glm::vec3 pos)
{
    //float step = 1.0f - glm::smoothstep(80.0f, 100.0f, glm::length(pos));
    float x = TetraApp::s_noise.eval(pos.x/150, pos.y/150, pos.z/150);
    x += 0.1f * TetraApp::s_noise.eval(pos.x/20, pos.y/20, pos.z/20);
    //return ((x + 1.0f) * step) - 1.0f;
    return x;
}

TetraApp::TetraApp() :
    m_drawCalls(32), m_triangles(32), m_vertices(32)
{
    m_defaultShader = new Shader("../shaders/Default.shader");
    m_wireframeShader = new Shader("../shaders/Wireframe.shader");
    m_camera = new Camera({0, 0, 15.0f}, 50.0f, 1000.0f, 45.0f, 
        WINDOW_PIXEL_WIDTH / (float)WINDOW_PIXEL_HEIGHT);
   
    m_drawOutline = false;
    m_outlineColor = {0.9, 0.3, 0.3};
    m_shallowMeshColor = {1, 1, 0};
    m_deepMeshColor = {1, 0, 1};

    // Create the tetra hierarchy.
    TetraHierarchy::Parameters params;
    params.maxLevel = 6;
    params.mcChunkDim = 4;
    params.splitFactor = 0.4f;
    params.maxDistance = 500.0f;
    uint32_t maxCoord = TetraHierarchy::MaxCoord(params.maxLevel);
    CubeGrid grid = CubeGrid(
        maxCoord+1,
        {-1000.0f, -1000.0f, -1000.0f},
        2000.0f);
    
    m_hierarchy = new TetraHierarchy(grid, Noise, params);
    // make sure we set the camera info before beggining
    // the split/merge thread.
    m_hierarchy->UpdateCamera(m_camera->WorldPosition(), m_camera->FrustrumPlanes());
    // do the split/merge in a background thread.
    m_worker = new std::thread([&]() {
        m_hierarchy->Work();
    });
    
    m_renderer->SetBackgroundColor({0.1, 0.1, 0.1, 1});
}

TetraApp::~TetraApp() 
{
    delete m_camera;
    delete m_defaultShader;
    delete m_wireframeShader;
    delete m_worker;
    delete m_hierarchy;    
}

void TetraApp::Update()
{
    bool cameraChanged = m_camera->Update(m_inputMgr);
    if (cameraChanged) {
        printf("\nUpdate camera\n");
        m_hierarchy->UpdateCamera(m_camera->WorldPosition(), m_camera->FrustrumPlanes());
    }
}

void TetraApp::DrawImGui() 
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
        ImGui::Begin("Application info"); 
        ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 
            1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate); 
        ImGui::Text("Average draw calls per frame : %.0f", m_drawCalls.GetAverage()); 
        ImGui::Text("Average vertex count : %.0f", m_vertices.GetAverage()); 
        ImGui::Text("Average triangle count : %.0f", m_triangles.GetAverage());

        ImGui::ColorEdit3("Mesh shallow color", (float*)&m_shallowMeshColor);
        ImGui::ColorEdit3("Mesh deep color", (float*)&m_deepMeshColor);

        ImGui::Text("Frustrum plane normals :");
        for (uint8_t i = 0; i < 6; i++) {
            const glm::vec3& n = m_camera->FrustrumPlanes()[i].GetNormal();
            ImGui::Text("%.2f %.2f %.2f", n.x, n.y, n.z);
        }

        ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());   
}

/*void TetraApp::DrawOutline()
{
    m_wireframeShader->Bind();
    m_wireframeShader->SetUniformMat4f("u_worldToView", m_camera->WorldToViewMatrix());
    m_wireframeShader->SetUniformMat4f("u_viewToScreen", m_camera->ViewToScreenMatrix()); 
    
    // wireframe outline
    GLCall(glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ));
    const glm::vec3& c = m_outlineColor;
    m_wireframeShader->SetUniform3f("u_color", c.x, c.y, c.z); 
    m_renderer->Draw(m_hierarchy->GetOutlineMesh(), *m_wireframeShader);
}*/

void TetraApp::DrawMesh() 
{
    m_defaultShader->Bind();
    m_defaultShader->SetUniformMat4f("u_worldToView", m_camera->WorldToViewMatrix());
    m_defaultShader->SetUniformMat4f("u_viewToScreen", m_camera->ViewToScreenMatrix()); 
    m_defaultShader->SetUniform3f("u_lightDirection", -1, 0, -1);
    GLCall(glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ));
    
    m_hierarchy->DrawMesh(m_renderer, m_defaultShader);

    /*Mesh* mesh = m_hierarchy->GetCurrentMesh();
    mesh->AllocateGPUBuffers(GL_STATIC_DRAW, mesh->GetVertexCount(), mesh->GetTriangleCount() * 3);
    mesh->UploadGPUBuffers();
    printf("\nGet Current Mesh\n");
    m_renderer->Draw(*mesh, *m_defaultShader);*/
           
    //m_drawCalls.AddSample(1);
    //m_triangles.AddSample(mesh->GetTriangleCount());
    //m_vertices.AddSample(mesh->GetVertexCount());
    //delete mesh;
}

void TetraApp::Render()
{
    DrawMesh();
    DrawImGui();
}
