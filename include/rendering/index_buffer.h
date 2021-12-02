#pragma once
#include <stdint.h>
#include <GL/glew.h>


class IndexBuffer
{
public:
    typedef uint32_t index_t;
    static inline GLenum s_glIndexType = GL_UNSIGNED_INT;

    // mode can be GL_STATIC_DRAW or GL_DYNAMIC_DRAW.
    // you can pass data=nullptr if you want to upload data later.
    IndexBuffer(GLenum mode, const void* data, size_t size);
    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;

    void UploadData(const void* data, size_t size);

    size_t GetSize() const;
private:
    size_t m_size;
    uint32_t m_rendererID;
};