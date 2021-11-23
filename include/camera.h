#pragma once
#include <glm/glm.hpp>


class Camera
{
public:
    // Initially, the camera looks down the z axis,
    // and the y axis is up.
    Camera(const glm::vec3& position, float speed);
    ~Camera();

    // Translate in the local camera axis : 
    // x is left
    // y is up
    // z is forward
    void Move(const glm::vec3& direction);
    glm::mat4x4 ProjectionMatrix(float FOVdev, float aspectRatio, float clipNear);
private:
    glm::vec3 m_position;
    glm::vec3 m_forward;
    glm::vec3 m_up;

    float m_speed;
};