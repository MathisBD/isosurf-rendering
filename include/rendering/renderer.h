#pragma once
#include "rendering/shader.h"
#include "rendering/vertex_array.h"

#include <GLFW/glfw3.h>


class Renderer
{
public:
    Renderer();
    void Draw(const VertexArray& va, const Shader& shader, unsigned int triangleCount) const;
    void Clear() const;
};