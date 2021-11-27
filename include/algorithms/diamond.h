#pragma once 
#include <glm/glm.hpp>
#include <vector>
#include "algorithms/tetra.h"
#include <assert.h>


typedef glm::u32vec3 vertex_t;


struct Diamond
{
public:
    // All the info can be retrieved from only the center position.
    // We cache it because there will likely be relatively few diamonds
    // (much less than triangles in the mesh).
    vertex_t center;
    uint8_t phase;
    uint8_t level;
    uint8_t scale;
    // The maximum level allowed in the tetra hierarchy.
    uint32_t maxLevel;
    
    std::vector<vertex_t> children;
    std::vector<vertex_t> parents;
    
    // The maximum tetrahedron count for a diamond
    // depends on its phase and if it is on the border of the grid.
    uint32_t maxTetraCount;
    std::vector<Tetra*> activeTetras;


    Diamond(const vertex_t& center_, uint32_t maxLevel_);

    inline bool IsComplete() const
    {
        return activeTetras.size() >= maxTetraCount;
    }
private:
    void InitChildren();
    void InitParents();
    
    void AddChild(const glm::i32vec3& ofs);
    void AddParent_0(const glm::i32vec3& ofs);
    void AddParent_1_2(const glm::i32vec3& ofs);

    uint8_t TrailingZeros(uint32_t x) const;    
    uint8_t CountOnBits(const vertex_t& v, uint8_t pos) const;
    bool IsBitOn(uint32_t x, uint32_t pos) const;
    bool IsAdditionValid(const vertex_t& v, const glm::i32vec3 ofs) const;
    // Count the number of coordinates of v that are
    // equal to zero or to MaxCoord();
    uint8_t LimitCoordsCount(const vertex_t& v);
};
