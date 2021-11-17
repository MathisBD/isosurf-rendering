#pragma once
#include <glm/glm.hpp>


// a simple orthographic camera,
// looking along the Z axis.
class Camera
{
public:
    glm::vec2 m_centerPos;
    float m_viewHeight; // the width is then given by the window aspect ratio
    float m_translationSpeed;
    Camera() : m_centerPos(0.0f, 0.0f), m_viewHeight(1.0f),
        m_translationSpeed(0.1f) {}
};