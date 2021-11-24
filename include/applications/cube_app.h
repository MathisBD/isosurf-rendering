#pragma once
#include "application.h"
#include "rendering/mesh.h"
#include "camera.h"
#include "rendering/shader.h"
#include "algorithms/marching_cubes.h"

class CubeApp : public Application
{
public:
    CubeApp();
    ~CubeApp();
protected:
    Camera* m_camera;
    Mesh* m_mesh;
    Shader* m_shader;
    MCChunk* m_mcChunk;
 
    // camera rotation
    bool m_rotateCamera = false;
    glm::vec2 m_prevCursorPos;

    
    void Update() override;
    void Render() override;
};