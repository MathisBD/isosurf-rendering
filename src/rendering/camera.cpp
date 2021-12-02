#include "rendering/camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "timer.h"
#include <assert.h>


Camera::Camera(const glm::vec3& position, float moveSpeed, float rotateSpeed,
    float fovDeg, float aspectRatio) :
    m_position(position), 
    m_forward(0.0f, 0.0f, -1.0f), 
    m_up(0.0f, 1.0f, 0.0f),
    m_moveSpeed(moveSpeed),
    m_rotateSpeed(rotateSpeed),
    m_fov(fovDeg),
    m_aspectRatio(aspectRatio)
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

void Camera::Update(const InputManager* input) 
{

    // translate camera
    if (input->m_leftKey == KeyState::PRESSED) {
        Move({1.0, 0.0, 0.0});
    }
    if (input->m_rightKey == KeyState::PRESSED) {
        Move({-1.0, 0.0, 0.0});
    }
    if (input->m_downKey == KeyState::PRESSED) {
        Move({0.0, 0.0, -1.0});
    }
    if (input->m_upKey == KeyState::PRESSED) {
        Move({0.0, 0.0, 1.0});
    }
    if (input->m_rShiftKey == KeyState::PRESSED) {
        Move({0.0, 1.0, 0.0});
    }
    if (input->m_rCtrlKey == KeyState::PRESSED) {
        Move({0.0, -1.0, 0.0});
    }

    // rotate camera
    if (input->m_rightMouse == KeyState::PRESSED) {
        if (!m_rotating) {
            input->DisableCursor();
            m_prevCursorPos = input->CursorPosition();
        }
        m_rotating = true;
    }
    if (input->m_rightMouse == KeyState::RELEASED) {
        if (m_rotating) {
            input->ShowCursor();
        }
        m_rotating = false;
    }
    if (m_rotating) {
        glm::vec2 cursorPos = input->CursorPosition();
        RotateHorizontal(-(cursorPos.x - m_prevCursorPos.x));
        RotateVertical(-(cursorPos.y - m_prevCursorPos.y));
        m_prevCursorPos = cursorPos;
    }

    UpdateMatrix(
        m_fov, 
        m_aspectRatio,
        0.1f,
        1000.0f); 
}

void Camera::UpdateMatrix(float FOVdeg, float aspectRatio, float clipNear, float clipFar) 
{
    assert(FOVdeg > 0.0f);
    assert(aspectRatio > 0.0f);
    assert(clipNear > 0.0f);

    m_worldToView = glm::lookAt(m_position, m_position + m_forward, m_up);  
    m_viewToScreen = glm::perspective(glm::radians(FOVdeg), aspectRatio, clipNear, clipFar);

    UpdateFrustrumPlanes();
}

const glm::vec3 Camera::WorldPosition() const 
{
    return m_position;    
}

const glm::mat4& Camera::ViewToScreenMatrix() const
{
    return m_viewToScreen;    
}

const glm::mat4& Camera::WorldToViewMatrix() const
{
    return m_worldToView;    
}

const Plane* Camera::FrustrumPlanes() const 
{
    return m_frustrumPlanes;    
}

void Camera::UpdateFrustrumPlanes() 
{
    // We transpose so that m[i] refers to 
    // the i-th ROW of the original MVP matrix.
    // This is because GLM uses m[col][row] (by default).
    // See http://web.archive.org/web/20120531231005/http://crazyjoke.free.fr/doc/3D/plane%20extraction.pdf
    // for an explanation (nothing really fancy).
    glm::mat4 m = glm::transpose(m_viewToScreen * m_worldToView);
    m_frustrumPlanes[0] = Plane(- (m[4] + m[1])); // left
    m_frustrumPlanes[1] = Plane(- (m[4] - m[1])); // right
    m_frustrumPlanes[2] = Plane(- (m[4] + m[2])); // bottom
    m_frustrumPlanes[3] = Plane(- (m[4] - m[2])); // top
    m_frustrumPlanes[4] = Plane(- (m[4] + m[3])); // near
    m_frustrumPlanes[5] = Plane(- (m[4] - m[3])); // far
}