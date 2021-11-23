#pragma once
#include <stdint.h>
#include <GL/glew.h>


class IndexBuffer
{
public:
    // an index should fit on 16 bits : we only support small meshes
    typedef uint32_t index_t;
    static inline GLenum s_glIndexType = GL_UNSIGNED_INT;

    IndexBuffer(const index_t* data, uint32_t count);
    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;

    uint32_t GetCount() const;
private:
    uint32_t m_count;
    uint32_t m_rendererID;
};