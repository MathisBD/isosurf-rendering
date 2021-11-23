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
    glm::mat4x4 ProjectionMatrix(float FOVdev, float aspectRatio, float clipNear);
private:
    glm::vec3 m_position;
    glm::vec3 m_forward;
    glm::vec3 m_up;

    float m_moveSpeed;
    float m_rotateSpeed;

    glm::vec3 Left();
};