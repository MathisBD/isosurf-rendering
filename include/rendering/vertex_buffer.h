#pragma once
#include <vector>


class VertexBuffer
{
private:
    unsigned int m_rendererID;
public:
    VertexBuffer(void* data, size_t size);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;
};