#include "algorithms/diamond.h"
#include "algorithms/tetra_hierarchy.h"


template <typename T>
static T min3(T a, T b, T c)
{
    if (a <= b) {
        return a <= c ? a : c;
    }
    else {
        return b <= c ? b : c;
    }
}

void Diamond::Print() const
{
    printf("[+] DIAMOND\n");
    printf("\tcenter=(%u,%u,%u)", 
        center.x,
        center.y,
        center.z);

    printf("\tlevel=%u", level);
    printf("\tphase=%u", phase);
    printf("\tscale=%u\n", scale);
    
    printf("\tchildren=");
    for (const vertex_t& c : children) {
        printf("(%u,%u,%u) ", c.x, c.y, c.z);
    }
    printf("\n\tparents=");
    for (const vertex_t& p : parents) {
        printf("(%u,%u,%u) ", p.x, p.y, p.z);
    }
    printf("\ntetraCount=%lu/%u\n", activeTetras.size(), maxTetraCount);
}

Diamond::Diamond(const vertex_t& center_, uint8_t maxLevel_) 
{
    center = center_;
    maxLevel = maxLevel_;
    isSplit = false;
    lastCheck = 0;
    nextLeaf = nullptr;
    prevLeaf = nullptr;

    // Compute the diamond info.
    scale = min3(TrailingZeros(center.x), TrailingZeros(center.y), TrailingZeros(center.z));
    level = maxLevel - scale;
    switch (CountOnBits(center, scale)) {
    case 1: phase = 2; break;
    case 2: phase = 1; break;
    case 3: phase = 0; break;
    default: assert(false);
    }

    switch (phase) {
    case 0: maxTetraCount = 6; break;
    case 1: 
    { 
        switch (LimitCoordsCount(center)) {
        case 0: maxTetraCount = 4; break;
        case 1: maxTetraCount = 2; break;
        default: assert(false);
        }
        break;
    }
    case 2:
    {
        switch (LimitCoordsCount(center)) {
        case 0: maxTetraCount = 8; break;
        case 1: maxTetraCount = 4; break;
        case 2: maxTetraCount = 2; break;
        default: assert(false);  
        }
        break;
    }  
    default: assert(false);
    }
    
    InitChildren();
    InitParents();
}

// See the README.md file for an explanation.
void Diamond::InitChildren() 
{
    // No children for diamonds at max depth.
    if (level == maxLevel && phase == 2) {
        return;
    }

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
        assert(scale > 0);
        AddChild({  (1 << (scale-1)),  (1 << (scale-1)),  (1 << (scale-1)) });
        AddChild({  (1 << (scale-1)),  (1 << (scale-1)), -(1 << (scale-1)) });
        AddChild({  (1 << (scale-1)), -(1 << (scale-1)),  (1 << (scale-1)) });
        AddChild({  (1 << (scale-1)), -(1 << (scale-1)), -(1 << (scale-1)) });
        AddChild({ -(1 << (scale-1)),  (1 << (scale-1)),  (1 << (scale-1)) });
        AddChild({ -(1 << (scale-1)),  (1 << (scale-1)), -(1 << (scale-1)) });
        AddChild({ -(1 << (scale-1)), -(1 << (scale-1)),  (1 << (scale-1)) });
        AddChild({ -(1 << (scale-1)), -(1 << (scale-1)), -(1 << (scale-1)) });
        assert(children.size() <= 8);
        break;
    default: assert(false);
    }
}

// See the README.md file for an explanation.
void Diamond::InitParents() 
{
    // No parents for diamonds at 0 depth.
    if (level == 0 && phase == 0) {
        return;
    }

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

uint8_t Diamond::TrailingZeros(uint32_t x) const 
{
    for (uint8_t count = 0; count < 32; count++) {
        if (IsBitOn(x, count)) {
            return count;
        }
    }
    return 32;
}

inline uint8_t Diamond::CountOnBits(const vertex_t& v, uint8_t pos) const
{
    uint8_t count = 0;
    if (IsBitOn(v.x, pos)) { count++; }
    if (IsBitOn(v.y, pos)) { count++; }
    if (IsBitOn(v.z, pos)) { count++; }
    return count;
}

inline bool Diamond::IsBitOn(uint32_t x, uint8_t pos) const
{
    return (x & (1 << pos)) ? true : false;  
}

inline bool Diamond::IsAdditionValid(const vertex_t& v, const glm::i32vec3 ofs) const
{
    // All vertices should fit into a signed integer anyways,
    // so we can cast without taking any special care.
    glm::i32vec3 add = glm::i32vec3(v) + ofs;
    int32_t maxCoord = TetraHierarchy::MaxCoord(maxLevel);
    return 0 <= add.x && add.x <= maxCoord &&
           0 <= add.y && add.y <= maxCoord &&
           0 <= add.z && add.z <= maxCoord;
}

uint8_t Diamond::LimitCoordsCount(const vertex_t& v) 
{
    uint8_t count = 0;
    uint32_t maxCoord = TetraHierarchy::MaxCoord(maxLevel);
    if (v.x == 0 || v.x == maxCoord) { count++; }
    if (v.y == 0 || v.y == maxCoord) { count++; }
    if (v.z == 0 || v.z == maxCoord) { count++; }
    return count;
}