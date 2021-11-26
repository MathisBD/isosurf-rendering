#include "algorithms/diamond.h"


// See the README.md file for an explanation.
void Diamond::InitChildren() 
{
    switch (phase) {
    case 0:
        AddChild({ 1 << scale, 0, 0 });
        AddChild({ -(1 << scale), 0, 0 });
        AddChild({ 0, 1 << scale, 0 });
        AddChild({ 0, -(1 << scale), 0 });
        AddChild({ 0, 0, 1 << scale });
        AddChild({ 0, 0, -(1 << scale) });
        assert(children.size() <= 6);
        break;
    case 1:
        if (IsBitOn(center.x, scale)) {
            AddChild({ 1 << scale, 0, 0 });
            AddChild({ -(1 << scale), 0, 0 });
        }
        if (IsBitOn(center.y, scale)) {
            AddChild({ 0, 1 << scale, 0 });
            AddChild({ 0, -(1 << scale), 0 });
        }
        if (IsBitOn(center.z, scale)) {
            AddChild({ 0, 0, 1 << scale });
            AddChild({ 0, 0, -(1 << scale) });
        }
        assert(children.size() <= 4);
        break;
    case 2:
        AddChild({  (1 << scale),  (1 << scale),  (1 << scale) });
        AddChild({  (1 << scale),  (1 << scale), -(1 << scale) });
        AddChild({  (1 << scale), -(1 << scale),  (1 << scale) });
        AddChild({  (1 << scale), -(1 << scale), -(1 << scale) });
        AddChild({ -(1 << scale),  (1 << scale),  (1 << scale) });
        AddChild({ -(1 << scale),  (1 << scale), -(1 << scale) });
        AddChild({ -(1 << scale), -(1 << scale),  (1 << scale) });
        AddChild({ -(1 << scale), -(1 << scale), -(1 << scale) });
        assert(children.size() <= 8);
        break;
    default: assert(false);
    }
}

// See the README.md file for an explanation.
void Diamond::InitParents() 
{
    assert(depth > 0);

    switch (phase) {
    case 0:
        AddParent_0({  (1 << scale),  (1 << scale),  (1 << scale) });
        AddParent_0({  (1 << scale),  (1 << scale), -(1 << scale) });
        AddParent_0({  (1 << scale), -(1 << scale),  (1 << scale) });
        AddParent_0({  (1 << scale), -(1 << scale), -(1 << scale) });
        AddParent_0({ -(1 << scale),  (1 << scale),  (1 << scale) });
        AddParent_0({ -(1 << scale),  (1 << scale), -(1 << scale) });
        AddParent_0({ -(1 << scale), -(1 << scale),  (1 << scale) });
        AddParent_0({ -(1 << scale), -(1 << scale), -(1 << scale) });
        assert(parents.size() <= 3);
        break;
    case 1:
        if (!IsBitOn(center.x, scale)) {
            AddParent_1_2({ 1 << scale, 0, 0 });
            AddParent_1_2({ -(1 << scale), 0, 0 });
        }
        if (!IsBitOn(center.y, scale)) {
            AddParent_1_2({ 0, 1 << scale, 0 });
            AddParent_1_2({ 0, -(1 << scale), 0 });
        }
        if (!IsBitOn(center.z, scale)) {
            AddParent_1_2({ 0, 0, 1 << scale });
            AddParent_1_2({ 0, 0, -(1 << scale) });
        }
        assert(parents.size() <= 2);
        break;
    case 2:
        if (!IsBitOn(center.x, scale)) {
            AddParent_1_2({ 1 << scale, 0, 0 });
            AddParent_1_2({ -(1 << scale), 0, 0 });
        }
        if (!IsBitOn(center.y, scale)) {
            AddParent_1_2({ 0, 1 << scale, 0 });
            AddParent_1_2({ 0, -(1 << scale), 0 });
        }
        if (!IsBitOn(center.z, scale)) {
            AddParent_1_2({ 0, 0, 1 << scale });
            AddParent_1_2({ 0, 0, -(1 << scale) });
        }
        assert(parents.size() <= 4);
        break;
    default: assert(false);
    }
}

inline void Diamond::AddParent_0(const glm::i32vec3& ofs) 
{
    if (IsAdditionValid(center, ofs)) {
        vertex_t p = vertex_t(
            center.x + ofs.x,
            center.y + ofs.y,
            center.z + ofs.z);
        if (CountOnBits(p, 1 + scale) == 1) {
            parents.push_back(p);
        }
    }
}

inline void Diamond::AddParent_1_2(const glm::i32vec3& ofs) 
{
    if (IsAdditionValid(center, ofs)) {
        parents.emplace_back(
            center.x + ofs.x, 
            center.y + ofs.y,
            center.z + ofs.z);
    }
}

inline void Diamond::AddChild(const glm::i32vec3& ofs) 
{
    if (IsAdditionValid(center, ofs)) {
        children.emplace_back(
            center.x + ofs.x, 
            center.y + ofs.y,
            center.z + ofs.z);
    }
}

inline uint32_t Diamond::CountOnBits(const vertex_t& v, uint32_t pos) 
{
    uint32_t count = 0;
    if (IsBitOn(v.x, pos)) { count++; }
    if (IsBitOn(v.y, pos)) { count++; }
    if (IsBitOn(v.z, pos)) { count++; }
    return count;
}

inline bool Diamond::IsBitOn(uint32_t x, uint32_t pos) 
{
    return x & (1 << pos) ? true : false;  
}

inline bool Diamond::IsAdditionValid(const vertex_t& v, const glm::i32vec3 ofs) 
{
    // All vertices should fit into a signed integer anyways,
    // so we can cast without taking any special care.
    glm::i32vec3 add = glm::i32vec3(v) + ofs;
    return 0 <= add.x && add.x <= (int32_t)maxCoord &&
           0 <= add.y && add.y <= (int32_t)maxCoord &&
           0 <= add.z && add.z <= (int32_t)maxCoord;
}