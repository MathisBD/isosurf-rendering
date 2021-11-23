#include "rendering/shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <assert.h>
#include "rendering/gl_errors.h"
#include <glm/gtc/type_ptr.hpp>


Shader::Shader(const std::string& filePath) : m_filePath(filePath)
{
    const ShaderSource& source = LoadSource();
    m_rendererID = CreateProgram(source);
}

Shader::~Shader() 
{
    GLCall(glDeleteProgram(m_rendererID));    
}

void Shader::Bind() const 
{
    GLCall(glUseProgram(m_rendererID));    
}

void Shader::Unbind() const 
{
    GLCall(glUseProgram(0));
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
    GLCall(unsigned int programID = glCreateProgram());
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, source.vertexSource);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, source.fragmentSource);

    GLCall(glAttachShader(programID, vs));
    GLCall(glAttachShader(programID, fs));
    GLCall(glLinkProgram(programID));
    GLCall(glValidateProgram(programID));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return programID;
}


unsigned int Shader::CompileShader(unsigned int type, const std::string& source) 
{
    // create the shader
    GLCall(unsigned int shaderID = glCreateShader(type));
    const char* src = source.c_str();
    GLCall(glShaderSource(shaderID, 1, &src, NULL));
    
    // compile the shader
    GLCall(glCompileShader(shaderID));
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
        
        GLCall(glDeleteShader(shaderID));
        return 0;
    }
    return shaderID;
}

void Shader::SetUniform2f(const std::string& name, float v0, float v1) 
{
    GLCall(glUniform2f(GetUniformLocation(name), v0, v1));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4x4& mat) 
{
    GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat)));    
}

int Shader::GetUniformLocation(const std::string& name) 
{
    if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end()) {
        return m_uniformLocationCache[name];
    }

    GLCall(int location = glGetUniformLocation(m_rendererID, name.c_str()));
    m_uniformLocationCache[name] = location;
    return location;  
}