#pragma once
#include <glm/glm.hpp>

class Vertex
{
public:
    glm::vec3 m_position;
    glm::vec3 m_color;

    Vertex(glm::vec3 position, glm::vec3 color) :
        m_position(position), m_color(color) { }
        
    ~Vertex() { }
};