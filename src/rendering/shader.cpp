#include "rendering/shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <assert.h>


Shader::Shader(const std::string& filePath) : m_filePath(filePath)
{
    const ShaderSource& source = LoadSource();
    m_rendererID = CreateProgram(source);
}

Shader::~Shader() 
{
    glDeleteProgram(m_rendererID);    
}

void Shader::Bind() const 
{
    glUseProgram(m_rendererID);    
}

void Shader::Unbind() const 
{
    glUseProgram(0);
}

Shader::ShaderSource Shader::LoadSource() 
{
    std::ifstream file;
    file.open(m_filePath);
    assert(file.good());

    enum ShaderType
    {
        NONE = -1, VERTEX, FRAGMENT, COUNT
    };

    std::string line;
    std::stringstream ss[ShaderType::COUNT];
    ShaderType sType = ShaderType::NONE;
    while (getline(file, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                sType = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                sType = ShaderType::FRAGMENT;
            }
            else {
                sType = ShaderType::NONE;
            }
        }
        else {
            assert(sType != ShaderType::NONE);
            ss[sType] << line << "\n";
        }
    }
    return {ss[ShaderType::VERTEX].str(), ss[ShaderType::FRAGMENT].str()};
}

unsigned int Shader::CreateProgram(const ShaderSource& source) 
{
    unsigned int programID = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, source.vertexSource);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, source.fragmentSource);

    glAttachShader(programID, vs);
    glAttachShader(programID, fs);
    glLinkProgram(programID);
    glValidateProgram(programID);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return programID;
}


unsigned int Shader::CompileShader(unsigned int type, const std::string& source) 
{
    // create the shader
    unsigned int shaderID = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shaderID, 1, &src, NULL);
    
    // compile the shader
    glCompileShader(shaderID);
    GLint status;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        // print the error log
        int logLength;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
        
        char* log = new char[logLength];
        glGetShaderInfoLog(shaderID, logLength, &logLength, log);
        
        std::cout << "[-] Shader compilation error in " << m_filePath << std::endl; 
        std::cout << "\t" << log << std::endl;
        
        glDeleteShader(shaderID);
        return 0;
    }
    return shaderID;
}

void Shader::SetUniform2f(const std::string& name, float v0, float v1) 
{
    glUniform2f(GetUniformLocation(name), v0, v1);
}

int Shader::GetUniformLocation(const std::string& name) 
{
    int location = glGetUniformLocation(m_rendererID, name.c_str());
    return location;  
}