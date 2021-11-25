#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "timer.h"
#include <assert.h>


Camera::Camera(const glm::vec3& position, float moveSpeed, float rotateSpeed) :
    m_position(position), 
    m_forward(0.0f, 0.0f, -1.0f), 
    m_up(0.0f, 1.0f, 0.0f),
    m_moveSpeed(moveSpeed),
    m_rotateSpeed(rotateSpeed)
{
}

Camera::~Camera() 
{
}

glm::vec3 Camera::Left() 
{
    return -glm::normalize(glm::cross(m_forward, m_up));       
}

void Camera::Move(const glm::vec3& direction) 
{
    glm::vec3 dir = glm::normalize(direction);
    m_position += Timer::s_dt * m_moveSpeed * dir.x * Left();
    m_position += Timer::s_dt * m_moveSpeed * dir.y * m_up;
    m_position += Timer::s_dt * m_moveSpeed * dir.z * m_forward;
}

void Camera::RotateHorizontal(float x) 
{
    float angle = glm::radians(Timer::s_dt * m_rotateSpeed * x);
    m_forward = glm::rotate(m_forward, angle, m_up);    
}

void Camera::RotateVertical(float y) 
{
    float angle = glm::radians(Timer::s_dt * m_rotateSpeed * y);
    glm::vec3 newForward = glm::rotate(m_forward, angle, Left());  
    if (glm::angle(newForward, m_up) > glm::radians(5.0f) &&
        glm::angle(newForward, -m_up) > glm::radians(5.0f)) {
        m_forward = newForward;
    }  
}

glm::mat4x4 Camera::ProjectionMatrix(float FOVdeg, float aspectRatio, float clipNear, float clipFar) 
{
    assert(FOVdeg > 0.0f);
    assert(aspectRatio > 0.0f);
    assert(clipNear > 0.0f);

    glm::mat4 view = glm::lookAt(m_position, m_position + m_forward, m_up);  
    glm::mat4 proj = glm::perspective(glm::radians(FOVdeg), aspectRatio, clipNear, clipFar);
    return proj * view; 
}