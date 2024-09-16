#include "MaterialInstance.h"
#include "Log.h"
#include <cstdint>

namespace Doodle
{

MaterialInstance::MaterialInstance(std::shared_ptr<Material> material) : m_material(material)
{
}

void MaterialInstance::Bind()
{
    // 绑定基础材质
    m_material->Bind();

    // 直接使用实例的统一值
    ApplyInstanceUniforms();
}

void MaterialInstance::Unbind()
{
    m_material->Unbind();
}

void MaterialInstance::SetUniform1f(const std::string &name, float value)
{
    m_instanceUniforms1f[name] = value;
}

void MaterialInstance::SetUniform2f(const std::string &name, glm::vec2 value)
{
    m_instanceUniforms2f[name] = value;
}

void MaterialInstance::SetUniform3f(const std::string &name, glm::vec3 value)
{
    m_instanceUniforms3f[name] = value;
}

void MaterialInstance::SetUniform4f(const std::string &name, glm::vec4 value)
{
    m_instanceUniforms4f[name] = value;
}

void MaterialInstance::SetUniform1i(const std::string &name, int value)
{
    m_instanceUniforms1i[name] = value;
}

void MaterialInstance::SetUniform2i(const std::string &name, glm::ivec2 value)
{
    m_instanceUniforms2i[name] = value;
}

void MaterialInstance::SetUniform3i(const std::string &name, glm::ivec3 value)
{
    m_instanceUniforms3i[name] = value;
}

void MaterialInstance::SetUniform4i(const std::string &name, glm::ivec4 value)
{
    m_instanceUniforms4i[name] = value;
}

void MaterialInstance::SetUniformMatrix4f(const std::string &name, glm::mat4 value)
{
    m_instanceUniforms4m[name] = value;
}

void MaterialInstance::SetUniformMatrix3f(const std::string &name, glm::mat3 value)
{
    m_instanceUniforms3m[name] = value;
}

void MaterialInstance::SetUniformTexture(const std::string &name, std::shared_ptr<Texture> value)
{
    m_instanceTextures[name] = value;
}

void MaterialInstance::ApplyInstanceUniforms()
{
    for (auto &[name, texture] : m_instanceTextures)
    {
        m_material->m_shader->SetUniformTexture(name, texture);
    }
    for (const auto &[name, value] : m_instanceUniforms1f)
    {
        m_material->m_shader->SetUniform1f(name, value);
    }
    for (const auto &[name, value] : m_instanceUniforms2f)
    {
        m_material->m_shader->SetUniform2f(name, value);
    }
    for (const auto &[name, value] : m_instanceUniforms3f)
    {
        m_material->m_shader->SetUniform3f(name, value);
    }
    for (const auto &[name, value] : m_instanceUniforms4f)
    {
        m_material->m_shader->SetUniform4f(name, value);
    }
    for (const auto &[name, value] : m_instanceUniforms1i)
    {
        m_material->m_shader->SetUniform1i(name, value);
    }
    for (const auto &[name, value] : m_instanceUniforms2i)
    {
        m_material->m_shader->SetUniform2i(name, value[0], value[1]);
    }
    for (const auto &[name, value] : m_instanceUniforms3i)
    {
        m_material->m_shader->SetUniform3i(name, value[0], value[1], value[2]);
    }
    for (const auto &[name, value] : m_instanceUniforms4i)
    {
        m_material->m_shader->SetUniform4i(name, value[0], value[1], value[2], value[3]);
    }
    for (const auto &[name, value] : m_instanceUniforms4m)
    {
        m_material->m_shader->SetUniformMatrix4f(name, value);
    }
    for (const auto &[name, value] : m_instanceUniforms3m)
    {
        m_material->m_shader->SetUniformMatrix3f(name, value);
    }
}

} // namespace Doodle