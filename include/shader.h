#pragma once
#include <string>
#include <GL/glew.h>

class Shader
{
private: 
    std::string m_sourceFilePath;
public:
    GLuint glid;
    Shader(GLenum type, const std::string& sourceFilePath);
    void compile();
};