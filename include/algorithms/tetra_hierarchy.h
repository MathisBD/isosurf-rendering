#pragma once
#include <unordered_map>
#include <glm/glm.hpp>
#include <vector>


struct Tetra
{
    glm::u32vec3 vertices[4];
};

// We only keep track of active diamonds.
// A diamond is active if it has at least one active tetra.
struct Diamond
{
    // only the active tetras of the diamond.
    std::vector<Tetra> tetras;
    // all the active parents
    std::vector<Diamond*> parents;
    // all the active children
    std::vector<Diamond*> children;
};

class TetraHierarchy
{
public:
    TetraHierarchy(uint32_t maxDepth);
    ~TetraHierarchy();

private:
    uint32_t m_maxDepth;
    std::unordered_map<glm::u32vec3, Diamond> m_diamonds;

    bool IsComplete(const Diamond& diamond, uint32_t depth);
    void ForceSplit(Diamond& diamond, uint32_t depth);
    void PredicateSplit(Diamond& diamond, bool (*predicate)(const Diamond&), uint32_t depth);
};
