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
    uint8_t depth;
    uint8_t phase;
    uint8_t level;
    uint8_t scale;
    // The maximum coordinate in the tetra hierarchy.
    uint32_t maxCoord;
    std::vector<vertex_t> children;
    std::vector<vertex_t> parents;
    std::vector<Tetra*> activeTetras;

    Diamond(const vertex_t& center, uint32_t depth, uint32_t maxCoord);

    inline bool IsComplete() const
    {
        {
            switch (phase) {
            case 0: return activeTetras.size() >= 6;
            case 1: return activeTetras.size() >= 4;
            case 2: return activeTetras.size() >= 8;
            }
            assert(false);
            return false;
        }
    }
private:
    void InitChildren();
    void InitParents();
    
    void AddChild(const glm::i32vec3& ofs);
    void AddParent_0(const glm::i32vec3& ofs);
    void AddParent_1_2(const glm::i32vec3& ofs);
    
    uint32_t CountOnBits(const vertex_t& v, uint32_t pos);
    bool IsBitOn(uint32_t x, uint32_t pos);
    bool IsAdditionValid(const vertex_t& v, const glm::i32vec3 ofs);
};
