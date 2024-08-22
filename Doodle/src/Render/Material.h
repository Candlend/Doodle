#pragma once

#include "pch.h"
#include <glm/glm.hpp>

#include "Shader.h"
#include "ShaderLibrary.h"
#include "Texture.h"

namespace Doodle
{

class MaterialInstance;

class DOO_API Material
{
    friend class MaterialInstance;

public:
    static std::shared_ptr<Material> Create(const std::string &shaderName)
    {
        return std::make_shared<Material>(shaderName);
    }

    static std::shared_ptr<Material> Create(std::shared_ptr<Shader> shader)
    {
        return std::make_shared<Material>(shader);
    }

    explicit Material(std::shared_ptr<Shader> shader) : m_shader(shader)
    {
    }

    explicit Material(const std::string &shaderName)
    {
        m_shader = ShaderLibrary::Get().GetShader(shaderName);
    }

    void Bind();

    void SetUniform1f(const std::string &name, float value);
    void SetUniform2f(const std::string &name, glm::vec2 value);
    void SetUniform3f(const std::string &name, glm::vec3 value);
    void SetUniform4f(const std::string &name, glm::vec4 value);
    void SetUniform1i(const std::string &name, int value);
    void SetUniform2i(const std::string &name, glm::ivec2 value);
    void SetUniform3i(const std::string &name, glm::ivec3 value);
    void SetUniform4i(const std::string &name, glm::ivec4 value);
    void SetUniformMatrix3f(const std::string &name, glm::mat3 value);
    void SetUniformMatrix4f(const std::string &name, glm::mat4 value);
    void SetUniformTexture(const std::string &name, std::shared_ptr<Texture> value);

private:
    std::shared_ptr<Shader> m_shader;
    std::map<std::string, std::shared_ptr<Texture>> m_textures;
    std::map<std::string, uint32_t> m_textureSlots;
    std::map<std::string, float> m_uniforms1f;
    std::map<std::string, glm::vec2> m_uniforms2f;
    std::map<std::string, glm::vec3> m_uniforms3f;
    std::map<std::string, glm::vec4> m_uniforms4f;
    std::map<std::string, float> m_uniforms1i;
    std::map<std::string, glm::ivec2> m_uniforms2i;
    std::map<std::string, glm::ivec3> m_uniforms3i;
    std::map<std::string, glm::ivec4> m_uniforms4i;
    std::map<std::string, glm::mat3> m_uniforms3m;
    std::map<std::string, glm::mat4> m_uniforms4m;
};

} // namespace Doodle
