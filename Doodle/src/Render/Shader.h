#pragma once

#include "Renderer.h"
#include <glm/glm.hpp>

namespace Doodle
{

class DOO_API Shader
{
public:
    static std::shared_ptr<Shader> Create(const std::string &filepath);
    virtual void Bind() = 0;

    virtual void SetUniform1i(const std::string &name, int v) = 0;
    virtual void SetUniform2i(const std::string &name, int v1, int v2) = 0;
    virtual void SetUniform3i(const std::string &name, int v1, int v2, int v3) = 0;
    virtual void SetUniform4i(const std::string &name, int v1, int v2, int v3, int v4) = 0;
    virtual void SetUniform1f(const std::string &name, float v) = 0;
    virtual void SetUniform2f(const std::string &name, float v1, float v2) = 0;
    void SetUniform2f(const std::string &name, glm::vec2 v) {
        SetUniform2f(name, v.x, v.y);
    }
    virtual void SetUniform3f(const std::string &name, float v1, float v2, float v3) = 0;
    void SetUniform3f(const std::string &name, glm::vec3 v) {
        SetUniform3f(name, v.x, v.y, v.z);
    }
    virtual void SetUniform4f(const std::string &name, float v1, float v2, float v3, float v4) = 0;
    void SetUniform4f(const std::string &name, glm::vec4 v) {
        SetUniform4f(name, v.x, v.y, v.z, v.w);
    }
    virtual void SetUniform1iv(const std::string &name, int count, int *v) = 0;
    virtual void SetUniform2iv(const std::string &name, int count, int *v) = 0;
    virtual void SetUniform3iv(const std::string &name, int count, int *v) = 0;
    virtual void SetUniform4iv(const std::string &name, int count, int *v) = 0;
    virtual void SetUniform1fv(const std::string &name, int count, float *v) = 0;
    virtual void SetUniform2fv(const std::string &name, int count, float *v) = 0;
    virtual void SetUniform3fv(const std::string &name, int count, float *v) = 0;
    virtual void SetUniform4fv(const std::string &name, int count, float *v) = 0;
    virtual void SetUniformMatrix2f(const std::string &name, const glm::mat2 &mat) = 0;
    virtual void SetUniformMatrix3f(const std::string &name, const glm::mat3 &mat) = 0;
    virtual void SetUniformMatrix4f(const std::string &name, const glm::mat4 &mat) = 0;
};

} // namespace Doodle