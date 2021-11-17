#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>


Shader::Shader(GLenum type, const std::string& sourceFilePath) : 
    m_sourceFilePath(sourceFilePath)
{
    glid = glCreateShader(type);

    // get the shader source code
    std::ifstream file;
    file.open(sourceFilePath);
    std::stringstream ss(std::ios_base::out | std::ios_base::app);
    if (file.is_open()) {
        while (file.good()) {
            std::string line;
            getline(file, line);
            ss << line << "\n";
        }
    }
    std::string sourceCode = ss.str();
    
    // load it in the shader
    const char* str = sourceCode.c_str();
    glShaderSource(glid, 1, &str, NULL);
}

void Shader::compile()
{
    glCompileShader(glid);

    // check for compile errors
    GLint status;
    glGetShaderiv(glid, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        // print the error log
        char buffer[512];
        glGetShaderInfoLog(glid, 512, NULL, buffer);
        printf("[-] Shader compilation error in %s\n%s", 
            m_sourceFilePath.c_str(), buffer);
    }
}