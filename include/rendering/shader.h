#pragma once
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <unordered_map>

class Shader
{
public:
    Shader(const std::string& filePath);
    ~Shader();

    void SetUniform2f(const std::string& name, float v0, float v1);
    void SetUniform3f(const std::string& name, float v0, float v1, float v2);
    void SetUniformMat4f(const std::string& name, const glm::mat4x4& mat);

    void Bind() const;
    void Unbind() const;
private:
    struct ShaderSource
    {
        std::string vertexSource;
        std::string fragmentSource;
    };

    unsigned int m_rendererID;
    std::string m_filePath;
    std::unordered_map<std::string, int> m_uniformLocationCache;

    ShaderSource LoadSource();
    unsigned int CreateProgram(const ShaderSource& source);
    unsigned int CompileShader(unsigned int type, const std::string& source);

    int GetUniformLocation(const std::string& name);
};