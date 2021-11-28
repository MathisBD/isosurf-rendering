#pragma once
#include <glm/glm.hpp>


// A grid on a hexahedron. We subdivide
// the hexahedron using trilinear interpolation.
struct HexaGrid
{
public:
    // number of vertices along each axis.
    uint32_t dim;
    // For x,y,z = 0 or 1, the corner at (x,y,z) is
    // stored at corners[0bxyz].
    glm::vec3 corners[8];

    HexaGrid(uint32_t dim_, const glm::vec3 corners_[8]) 
    {
        dim = dim_;
        for (uint8_t i = 0; i < 8; i++) {
            corners[i] = corners_[i];
        }
    }

    inline static uint32_t Index3D(const glm::u32vec3& v, uint32_t dim)
    {
        return v.x + dim * v.y + dim * dim * v.z;
    }

    glm::vec3 WorldPosition(const glm::u32vec3& pos) const
    {
        // Use trilinear interpolation between the corners
        float x = pos.x / (float)(dim-1);
        float y = pos.y / (float)(dim-1);
        float z = pos.z / (float)(dim-1);

        glm::vec3 c00 = corners[0b000] * (1 - x) + corners[0b100] * x;
        glm::vec3 c01 = corners[0b001] * (1 - x) + corners[0b101] * x;
        glm::vec3 c10 = corners[0b010] * (1 - x) + corners[0b110] * x;
        glm::vec3 c11 = corners[0b011] * (1 - x) + corners[0b111] * x;

        glm::vec3 c0 = c00 * (1 - y) + c10 * y;
        glm::vec3 c1 = c01 * (1 - y) + c11 * y;

        return c0 * (1 - z) + c1 * z;
    }
};
