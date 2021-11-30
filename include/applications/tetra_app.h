#pragma once
#include "applications/application.h"
#include "rendering/mesh.h"
#include "rendering/camera.h"
#include "rendering/shader.h"
#include "algorithms/marching_cubes.h"
#include "algorithms/tetra_hierarchy.h"
#include "third_party/open_simplex_noise/noise.h"



class TetraApp : public Application
{
public:
    inline const static OpenSimplexNoise::Noise s_noise = {42};

    TetraApp();
    ~TetraApp();
protected:
    Camera* m_camera = nullptr;
    Shader* m_defaultShader = nullptr;
    Shader* m_wireframeShader = nullptr;
    TetraHierarchy* m_hierarchy = nullptr;
    
    // camera rotation
    bool m_rotateCamera = false;
    glm::vec2 m_prevCursorPos;

    // user controled parameters
    bool m_drawOutline;
    glm::vec3 m_outlineColor;
    glm::vec3 m_deepMeshColor;
    glm::vec3 m_shallowMeshColor;

    void Update() override;
    void Render() override;

    void DrawImGui();
    void DrawOutline();
    void DrawMesh();
};