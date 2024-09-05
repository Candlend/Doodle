
#include "Material.h"
#include "Log.h"
#include <cstdint>

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
    if (!m_textureSlots.contains(value->GetUUID()))
    {
        m_textureSlots[value->GetUUID()] = m_textureSlots.size();
    }
    m_textures[name] = value;
}

void Material::Bind()
{
    m_shader->Bind();
    for (auto &[name, texture] : m_textures)
    {
        m_shader->SetUniformTexture(name, texture, m_textureSlots[texture->GetUUID()]);
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

} // namespace Doodle