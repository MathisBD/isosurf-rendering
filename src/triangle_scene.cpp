#include "triangle_scene.h"
#include "timer.h"


TriangleScene::TriangleScene()
{

}

const std::vector<Vertex>& TriangleScene::GetTriangles() const 
{
    return m_triangles;    
}

void TriangleScene::AddTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& color)
{
    m_triangles.push_back(Vertex(a, color));
    m_triangles.push_back(Vertex(b, color));
    m_triangles.push_back(Vertex(c, color));
}

void TriangleScene::AddLine(const glm::vec3& a, const glm::vec3& b, const glm::vec3& color, float width)
{
    glm::vec3 tangent = glm::normalize(b - a);
    glm::vec3 ortho(-tangent.y, tangent.x, 0.0f);
    
    glm::vec3 a1 = a + ortho * (width / 2);
    glm::vec3 a2 = a - ortho * (width / 2);

    glm::vec3 b1 = b + ortho * (width / 2);
    glm::vec3 b2 = b - ortho * (width / 2);

    AddTriangle(a1, a2, b1, color);
    AddTriangle(b1, b2, a2, color);
}

const Camera& TriangleScene::GetCamera() const 
{
    return m_camera;    
}

void TriangleScene::TranslateCamera(const glm::vec2& direction)
{
    m_camera.m_centerPos += direction * m_camera.m_translationSpeed * Timer::s_dt;
}

void TriangleScene::SetCameraTranslationSpeed(float speed) 
{
    m_camera.m_translationSpeed = speed;    
}

void TriangleScene::SetCameraViewHeight(float height) 
{
    m_camera.m_viewHeight = height;    
}