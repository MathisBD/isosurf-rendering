#pragma once
#include <vector>



class VertexBufferLayout
{
public:
    struct LayoutElement
    {
        unsigned int glType;
        unsigned int count;
    };

    VertexBufferLayout();
    
    template<typename T>
    void Push(unsigned int count);
    
    unsigned int GetStride() const;
    const std::vector<LayoutElement>& GetElements() const;

    static unsigned int SizeOfGlType(unsigned int glType);
private:
    std::vector<LayoutElement> m_elements;
    unsigned int m_stride;
};