
#include <glad/glad.h>

#include "Material.h"
#include "Renderer.h"

namespace Doodle
{

void Material::SetUniform1f(const std::string &name, float value)
{
    m_uniforms1f[name] = value;
}

void Material::SetUniform2f(const std::string &name, glm::vec2 value)
{
    m_uniforms2f[name] = value;
}

void Material::SetUniform3f(const std::string &name, glm::vec3 value)
{
    m_uniforms3f[name] = value;
}

void Material::SetUniform4f(const std::string &name, glm::vec4 value)
{
    m_uniforms4f[name] = value;
}

void Material::SetUniform1i(const std::string &name, int value)
{
    m_uniforms1i[name] = value;
}

void Material::SetUniform2i(const std::string &name, glm::vec<2, int> value)
{
    m_uniforms2i[name] = value;
}

void Material::SetUniform3i(const std::string &name, glm::vec<3, int> value)
{
    m_uniforms3i[name] = value;
}

void Material::SetUniform4i(const std::string &name, glm::vec<4, int> value)
{
    m_uniforms4i[name] = value;
}

void Material::SetUniformMatrix4f(const std::string &name, glm::mat4 value)
{
    m_uniforms4m[name] = value;
}

void Material::SetUniformMatrix3f(const std::string &name, glm::mat3 value)
{
    m_uniforms3m[name] = value;
}

void Material::SetUniformTexture(const std::string &name, std::shared_ptr<Texture> value)
{
    m_textures[name] = value;
}

void Material::SetUniformTexture(const std::string &name, uint64_t textureHandle)
{
    m_textureHandles[name] = textureHandle;
}

float Material::GetUniform1f(const std::string &name)
{
    return m_uniforms1f[name];
}

glm::vec2 Material::GetUniform2f(const std::string &name)
{
    return m_uniforms2f[name];
}

glm::vec3 Material::GetUniform3f(const std::string &name)
{
    return m_uniforms3f[name];
}

glm::vec4 Material::GetUniform4f(const std::string &name)
{
    return m_uniforms4f[name];
}

int Material::GetUniform1i(const std::string &name)
{
    return m_uniforms1i[name];
}

glm::ivec2 Material::GetUniform2i(const std::string &name)
{
    return m_uniforms2i[name];
}

glm::ivec3 Material::GetUniform3i(const std::string &name)
{
    return m_uniforms3i[name];
}

glm::ivec4 Material::GetUniform4i(const std::string &name)
{
    return m_uniforms4i[name];
}

glm::mat3 Material::GetUniformMatrix3f(const std::string &name)
{
    return m_uniforms3m[name];
}

glm::mat4 Material::GetUniformMatrix4f(const std::string &name)
{
    return m_uniforms4m[name];
}

std::shared_ptr<Texture> Material::GetUniformTexture(const std::string &name)
{
    return m_textures[name];
}

uint64_t Material::GetUniformTextureHandle(const std::string &name)
{
    return m_textureHandles[name];
}

void Material::Bind()
{
    m_shader->Bind();
    for (auto &[name, texture] : m_textures)
    {
        m_shader->SetUniformTexture(name, texture);
    }
    for (auto &[name, handle] : m_textureHandles)
    {
        m_shader->SetUniformTexture(name, handle);
    }
    for (auto &[name, value] : m_uniforms1f)
    {
        m_shader->SetUniform1f(name, value);
    }
    for (auto &[name, value] : m_uniforms2f)
    {
        m_shader->SetUniform2f(name, value);
    }
    for (auto &[name, value] : m_uniforms3f)
    {
        m_shader->SetUniform3f(name, value);
    }
    for (auto &[name, value] : m_uniforms4f)
    {
        m_shader->SetUniform4f(name, value);
    }
    for (auto &[name, value] : m_uniforms1i)
    {
        m_shader->SetUniform1i(name, value);
    }
    for (auto &[name, value] : m_uniforms2i)
    {
        m_shader->SetUniform2i(name, value[0], value[1]);
    }
    for (auto &[name, value] : m_uniforms3i)
    {
        m_shader->SetUniform3i(name, value[0], value[1], value[2]);
    }
    for (auto &[name, value] : m_uniforms4i)
    {
        m_shader->SetUniform4i(name, value[0], value[1], value[2], value[3]);
    }
    for (auto &[name, value] : m_uniforms4m)
    {
        m_shader->SetUniformMatrix4f(name, value);
    }
    for (auto &[name, value] : m_uniforms3m)
    {
        m_shader->SetUniformMatrix3f(name, value);
    }
}

void Material::Unbind()
{
}

} // namespace Doodle