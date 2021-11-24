#pragma once
#include "rendering/shader.h"
#include "rendering/vertex_array.h"
#include "rendering/index_buffer.h"
#include <GLFW/glfw3.h>


class Renderer
{
public:
    Renderer();
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    void Clear() const;
    void SetBackgroundColor(const glm::vec4& color);
};