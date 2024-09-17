#pragma once

#include "pch.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

#include "Renderer.h"
#include "Texture.h"

namespace Doodle
{

enum class ShaderPropertyType
{
    None = 0,
    Float,
    Float2,
    Float3,
    Float4,
    Mat3,
    Mat4,
    Int,
    Int2,
    Int3,
    Int4,
    Bool,
    Sampler2D,
    SamplerCube,
};

struct ShaderProperty
{
    std::string Name;
    ShaderPropertyType Type;
};

class DOO_API Shader
{
public:
    static std::shared_ptr<Shader> Create(const std::string &filepath);

    virtual std::string GetPath() const = 0;
    virtual void Reload() = 0;
    virtual void Bind() = 0;
    virtual uint32_t GetUniformLocation(const std::string &name) = 0;
    virtual uint32_t GetUniformBlockIndex(const std::string &name) = 0;
    virtual uint32_t GetUniformBlockBinding(const std::string &name) = 0;
    virtual void BindUniformBlock(const std::string &name, uint32_t slot) = 0;
    virtual void PrintActiveUniforms() = 0;
    virtual std::vector<ShaderProperty> GetProperties() = 0;
    virtual void PrintActiveUniformBlocks() = 0;
    virtual uint32_t GetRendererID() const = 0;

    template <typename Func, typename... Args> void SetUniform(const std::string &name, Func func, Args... args)
    {
        Bind();
        Renderer::Submit([=, this]() {
            uint32_t location = GetUniformLocation(name);
            func(location, args...);
            std::ostringstream oss;
            (void)std::initializer_list<int>{(oss << args << ", ", 0)...};
            if (location != -1)
                DOO_CORE_TRACE("Shader uniform set: <{0}> {1}", location, name);
        });
    }

    virtual void SetUniform1i(const std::string &name, int v) = 0;
    virtual void SetUniform2i(const std::string &name, int v1, int v2) = 0;
    virtual void SetUniform3i(const std::string &name, int v1, int v2, int v3) = 0;
    virtual void SetUniform4i(const std::string &name, int v1, int v2, int v3, int v4) = 0;
    virtual void SetUniform1f(const std::string &name, float v) = 0;
    virtual void SetUniform2f(const std::string &name, float v1, float v2) = 0;
    void SetUniform2f(const std::string &name, glm::vec2 v)
    {
        SetUniform2f(name, v.x, v.y);
    }
    virtual void SetUniform3f(const std::string &name, float v1, float v2, float v3) = 0;
    void SetUniform3f(const std::string &name, glm::vec3 v)
    {
        SetUniform3f(name, v.x, v.y, v.z);
    }
    virtual void SetUniform4f(const std::string &name, float v1, float v2, float v3, float v4) = 0;
    void SetUniform4f(const std::string &name, glm::vec4 v)
    {
        SetUniform4f(name, v.x, v.y, v.z, v.w);
    }
    virtual void SetUniform1iv(const std::string &name, int count, int *v) = 0;
    void SetUniform1iv(const std::string &name, std::vector<int> v)
    {
        SetUniform1iv(name, v.size(), v.data());
    }
    virtual void SetUniform2iv(const std::string &name, int count, int *v) = 0;
    virtual void SetUniform3iv(const std::string &name, int count, int *v) = 0;
    virtual void SetUniform4iv(const std::string &name, int count, int *v) = 0;
    virtual void SetUniform1fv(const std::string &name, int count, float *v) = 0;
    void SetUniform1fv(const std::string &name, std::vector<float> v)
    {
        SetUniform1fv(name, v.size(), v.data());
    }
    virtual void SetUniform2fv(const std::string &name, int count, float *v) = 0;
    void SetUniform2fv(const std::string &name, std::vector<glm::vec2> v)
    {
        SetUniform2fv(name, v.size() * 2, &v[0].x);
    }
    virtual void SetUniform3fv(const std::string &name, int count, float *v) = 0;
    void SetUniform3fv(const std::string &name, std::vector<glm::vec3> v)
    {
        SetUniform3fv(name, v.size() * 3, &v[0].x);
    }
    virtual void SetUniform4fv(const std::string &name, int count, float *v) = 0;
    void SetUniform4fv(const std::string &name, std::vector<glm::vec4> v)
    {
        SetUniform4fv(name, v.size() * 4, &v[0].x);
    }
    virtual void SetUniformMatrix2f(const std::string &name, const glm::mat2 &mat) = 0;
    virtual void SetUniformMatrix3f(const std::string &name, const glm::mat3 &mat) = 0;
    virtual void SetUniformMatrix4f(const std::string &name, const glm::mat4 &mat) = 0;

    virtual void SetUniformTexture(const std::string &name, std::shared_ptr<Texture> texture) = 0;
};

} // namespace Doodle