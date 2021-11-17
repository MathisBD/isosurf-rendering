#pragma once
#include <vector>
#include "vertex.h"


class Scene
{
public:
    std::vector<Vertex> m_triangles;
    Scene();
    void addTriangle(const Vertex& a, const Vertex& b, const Vertex& c);
};
