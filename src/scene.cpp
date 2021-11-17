#include "scene.h"

Scene::Scene()
{

}

void Scene::addTriangle(const Vertex& a, const Vertex& b, const Vertex& c)
{
    m_triangles.push_back(a);
    m_triangles.push_back(b);
    m_triangles.push_back(c);
}