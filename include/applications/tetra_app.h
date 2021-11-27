#pragma once
#include "applications/application.h"
#include "rendering/mesh.h"
#include "camera.h"
#include "rendering/shader.h"
#include "algorithms/marching_cubes.h"
#include "algorithms/tetra_hierarchy.h"


class TetraApp : public Application
{
public:
    TetraApp();
    ~TetraApp();
protected:
    Camera* m_camera = nullptr;
    Shader* m_shader = nullptr;
    TetraHierarchy* m_hierarchy = nullptr;

    // camera rotation
    bool m_rotateCamera = false;
    glm::vec2 m_prevCursorPos;

    
    void Update() override;
    void Render() override;
};