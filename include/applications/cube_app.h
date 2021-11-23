#pragma once
#include "application.h"
#include "rendering/mesh.h"
#include "camera.h"
#include "rendering/shader.h"

class CubeApp : public Application
{
public:
    CubeApp();
    ~CubeApp();
protected:
    Camera* m_camera;
    Mesh* m_mesh;
    Shader* m_shader;
    
    void Update() override;
    void Render() override;
};