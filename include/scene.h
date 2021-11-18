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
    // all points should have the same z coordinate
    void addTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& color);
    // all points should have the same z coordinate
    void addLine(const glm::vec3& a, const glm::vec3& b, const glm::vec3& color, float width);
    void translateCamera(const glm::vec2& direction);
};
