#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "timer.h"
#include <assert.h>


Camera::Camera(const glm::vec3& position, float speed) :
    m_position(position), 
    m_forward(0.0f, 0.0f, -1.0f), 
    m_up(0.0f, 0.1f, 0.0f),
    m_speed(speed)
{
}

Camera::~Camera() 
{
}

void Camera::Move(const glm::vec3& direction) 
{
    glm::vec3 dir = glm::normalize(direction);
    glm::vec3 left = -glm::cross(m_forward, m_up);
    m_position += Timer::s_dt * m_speed * dir.x * left;
    m_position += Timer::s_dt * m_speed * dir.y * m_up;
    m_position += Timer::s_dt * m_speed * dir.z * m_forward;
}

glm::mat4x4 Camera::ProjectionMatrix(float FOVdeg, float aspectRatio, float clipNear) 
{
    assert(FOVdeg > 0.0f);
    assert(aspectRatio > 0.0f);
    assert(clipNear > 0.0f);

    glm::mat4 view = glm::lookAt(m_position, m_position + m_forward, m_up);  
    glm::mat4 proj = glm::infinitePerspective(glm::radians(FOVdeg), aspectRatio, clipNear);
    return proj * view; 
}