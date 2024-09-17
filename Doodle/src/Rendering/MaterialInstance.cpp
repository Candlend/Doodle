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

void MaterialInstance::SetUniformTexture(const std::string &name, uint64_t textureHandle)
{
    m_instanceTextureHandles[name] = textureHandle;
}

float MaterialInstance::GetUniform1f(const std::string &name)
{
    if (m_instanceUniforms1f.contains(name))
    {
        return m_instanceUniforms1f[name];
    }
    return m_material->GetUniform1f(name);
}

glm::vec2 MaterialInstance::GetUniform2f(const std::string &name)
{
    if (m_instanceUniforms2f.contains(name))
    {
        return m_instanceUniforms2f[name];
    }
    return m_material->GetUniform2f(name);
}

glm::vec3 MaterialInstance::GetUniform3f(const std::string &name)
{
    if (m_instanceUniforms3f.contains(name))
    {
        return m_instanceUniforms3f[name];
    }
    return m_material->GetUniform3f(name);
}

glm::vec4 MaterialInstance::GetUniform4f(const std::string &name)
{
    if (m_instanceUniforms4f.contains(name))
    {
        return m_instanceUniforms4f[name];
    }
    return m_material->GetUniform4f(name);
}

int MaterialInstance::GetUniform1i(const std::string &name)
{
    if (m_instanceUniforms1i.contains(name))
    {
        return m_instanceUniforms1i[name];
    }
    return m_material->GetUniform1i(name);
}

glm::ivec2 MaterialInstance::GetUniform2i(const std::string &name)
{
    if (m_instanceUniforms2i.contains(name))
    {
        return m_instanceUniforms2i[name];
    }
    return m_material->GetUniform2i(name);
}

glm::ivec3 MaterialInstance::GetUniform3i(const std::string &name)
{
    if (m_instanceUniforms3i.contains(name))
    {
        return m_instanceUniforms3i[name];
    }
    return m_material->GetUniform3i(name);
}

glm::ivec4 MaterialInstance::GetUniform4i(const std::string &name)
{
    if (m_instanceUniforms4i.contains(name))
    {
        return m_instanceUniforms4i[name];
    }
    return m_material->GetUniform4i(name);
}

glm::mat3 MaterialInstance::GetUniformMatrix3f(const std::string &name)
{
    if (m_instanceUniforms3m.contains(name))
    {
        return m_instanceUniforms3m[name];
    }
    return m_material->GetUniformMatrix3f(name);
}

glm::mat4 MaterialInstance::GetUniformMatrix4f(const std::string &name)
{
    if (m_instanceUniforms4m.contains(name))
    {
        return m_instanceUniforms4m[name];
    }
    return m_material->GetUniformMatrix4f(name);
}

std::shared_ptr<Texture> MaterialInstance::GetUniformTexture(const std::string &name)
{
    if (m_instanceTextures.contains(name))
    {
        return m_instanceTextures[name];
    }
    return m_material->GetUniformTexture(name);
}

uint64_t MaterialInstance::GetUniformTextureHandle(const std::string &name)
{
    if (m_instanceTextureHandles.contains(name))
    {
        return m_instanceTextureHandles[name];
    }
    return m_material->GetUniformTextureHandle(name);
}

void MaterialInstance::ApplyInstanceUniforms()
{
    for (auto &[name, texture] : m_instanceTextures)
    {
        m_material->m_shader->SetUniformTexture(name, texture);
    }
    for (auto &[name, handle] : m_instanceTextureHandles)
    {
        m_material->m_shader->SetUniformTexture(name, handle);
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