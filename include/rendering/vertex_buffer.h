#pragma once
#include <vector>
#include "rendering/gl_errors.h"


class VertexBuffer
{
public:
    VertexBuffer(GLenum mode, const void* data, size_t size);
    ~VertexBuffer();

    void UploadData(const void* data, size_t size);

    void Bind() const;
    void Unbind() const;
private:
    unsigned int m_rendererID;
    size_t m_size;
};