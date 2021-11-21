#pragma once
#include <vector>
#include "vertex.h"
#include "camera.h"


class TriangleScene
{
public:
    TriangleScene();
    
    const std::vector<Vertex>& GetTriangles() const;
    // all points should have the same z coordinate
    void AddTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& color);
    // all points should have the same z coordinate
    void AddLine(const glm::vec3& a, const glm::vec3& b, const glm::vec3& color, float width);
    
    const Camera& GetCamera() const;
    void TranslateCamera(const glm::vec2& direction);
    void SetCameraViewHeight(float height);
    void SetCameraTranslationSpeed(float speed);
private:
    std::vector<Vertex> m_triangles;
    Camera m_camera;
};
