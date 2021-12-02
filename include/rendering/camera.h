#pragma once
#include <glm/glm.hpp>
#include "rendering/plane.h"
#include "input_mgr.h"


class Camera
{
public:
    // Initially, the camera looks down the z axis,
    // and the y axis is up.
    Camera(const glm::vec3& position, float moveSpeed, float rotateSpeed,
        float fovDeg, float aspectRatio);
    ~Camera();

    void Update(const InputManager* input);
    // Translate in the local camera axis : 
    // x is left
    // y is up
    // z is forward
    
    const glm::vec3 WorldPosition() const;
    const glm::mat4& WorldToViewMatrix() const;
    const glm::mat4& ViewToScreenMatrix() const; 
    const Plane* FrustrumPlanes() const;
private:
    glm::vec3 m_position;
    glm::vec3 m_forward;
    glm::vec3 m_up;

    glm::mat4 m_worldToView;
    glm::mat4 m_viewToScreen;

    // The frustrum planes in world space.
    Plane m_frustrumPlanes[6];

    float m_moveSpeed;
    float m_rotateSpeed;
    // Field of view in degrees.
    float m_fov; 
    // screen width / screen height
    float m_aspectRatio;

    bool m_rotating;
    glm::vec2 m_prevCursorPos;

    glm::vec3 Left();
    
    void Move(const glm::vec3& direction);
    void RotateHorizontal(float x);
    void RotateVertical(float y);

    void UpdateMatrix(float FOVdev, float aspectRatio, float clipNear, float clipFar);
    void UpdateFrustrumPlanes();
};