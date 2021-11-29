#pragma once
#include "applications/application.h"
#include "rendering/mesh.h"
#include "rendering/camera.h"
#include "rendering/shader.h"
#include "algorithms/marching_cubes.h"

class CubeApp : public Application
{
public:
    CubeApp();
    ~CubeApp();
protected:
    Camera* m_camera = nullptr;
    Mesh* m_mesh = nullptr;
    Shader* m_shader = nullptr;
    MCChunk* m_mcChunk = nullptr;
 
    // camera rotation
    bool m_rotateCamera = false;
    glm::vec2 m_prevCursorPos;

    
    void Update() override;
    void Render() override;
};