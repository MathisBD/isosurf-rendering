#pragma once
#include <vector>
#include "vertex.h"
#include "camera.h"


class Scene
{
public:
    std::vector<Vertex> m_triangles;
    Camera m_camera;
    Scene();
    void addTriangle(const Vertex& a, const Vertex& b, const Vertex& c);
    void translateCamera(const glm::vec2& direction);
};
