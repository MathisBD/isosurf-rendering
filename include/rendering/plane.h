#pragma once
#include <glm/glm.hpp>


// A plane is given by the equation ax+by+cz+d=0.
class Plane
{
public:
    static bool SphereIntersectsFrustrum(const Plane frustrum[6], const glm::vec3& center, float radius)
    {
        for (uint8_t i = 0; i < 6; i++) {
            const Plane& p = frustrum[i];
            float dist = p.SignedDistToPoint(center);
            // a negative distance means inside the frustrum.
            if (dist > radius) {
                return false;
            }
        }
        return true;
    }

    Plane() 
    {
        m_a = m_b = m_c = m_d = 0;
    }
    
    Plane(float a, float b, float c, float d)
    {
        float normalLength = glm::sqrt(a*a+b*b+c*c);     
        m_a = a / normalLength;     
        m_b = b / normalLength;     
        m_c = c / normalLength;        
        m_d = d / normalLength;
    }

    Plane(const glm::vec4& abcd) : Plane(abcd.x, abcd.y, abcd.z, abcd.w)
    {
    }

    Plane(const glm::vec3& normal, const glm::vec3& point)
    {
        // The plane equation is normal . ((x y z) - point) = 0
        float normalLength = glm::length(normal);    
        m_a = normal.x / normalLength;     
        m_b = normal.y / normalLength;     
        m_c = normal.z / normalLength;        
        m_d = -(glm::dot(normal, point)) / normalLength;
    }
    
    ~Plane() 
    {
    }

    glm::vec3 GetNormal() const
    {
        return { m_a, m_b, m_c };
    }

    float SignedDistToPoint(const glm::vec3& p) const
    {
        return m_a*p.x + m_b*p.y + m_c*p.z + m_d;
    }
private:
    float m_a, m_b, m_c, m_d;
};