#pragma once
#include <glm/glm.hpp>


class Camera
{
public:
    // Initially, the camera looks down the z axis,
    // and the y axis is up.
    Camera(const glm::vec3& position, float moveSpeed, float rotateSpeed);
    ~Camera();

    // Translate in the local camera axis : 
    // x is left
    // y is up
    // z is forward
    void Move(const glm::vec3& direction);
    void RotateHorizontal(float x);
    void RotateVertical(float y);
    void UpdateMatrix(float FOVdev, float aspectRatio, float clipNear, float clipFar);
    const glm::mat4& WorldToViewMatrix();
    const glm::mat4& ViewToScreenMatrix(); 
private:
    glm::vec3 m_position;
    glm::vec3 m_forward;
    glm::vec3 m_up;

    glm::mat4 m_worldToView;
    glm::mat4 m_viewToScreen;

    float m_moveSpeed;
    float m_rotateSpeed;

    glm::vec3 Left();
};