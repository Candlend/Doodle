#pragma once

#include "Material.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <string>

namespace Doodle
{

class DOO_API MaterialInstance
{
public:
    static std::shared_ptr<MaterialInstance> Create(std::shared_ptr<Material> material)
    {
        return std::make_shared<MaterialInstance>(material);
    }

    MaterialInstance() : m_material(nullptr)
    {
    }
    MaterialInstance(std::shared_ptr<Material> material);

    void Bind();

    // 设置实例特定的统一值
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

    std::shared_ptr<Material> GetMaterial() const
    {
        return m_material;
    }

    std::shared_ptr<Shader> GetShader()
    {
        return m_material->GetShader();
    }

private:
    std::shared_ptr<Material> m_material;
    std::map<std::string, std::shared_ptr<Texture>> m_instanceTextures;
    std::map<std::string, uint32_t> m_instanceTextureSlots;
    std::map<std::string, float> m_instanceUniforms1f;
    std::map<std::string, glm::vec2> m_instanceUniforms2f;
    std::map<std::string, glm::vec3> m_instanceUniforms3f;
    std::map<std::string, glm::vec4> m_instanceUniforms4f;
    std::map<std::string, int> m_instanceUniforms1i;
    std::map<std::string, glm::ivec2> m_instanceUniforms2i;
    std::map<std::string, glm::ivec3> m_instanceUniforms3i;
    std::map<std::string, glm::ivec4> m_instanceUniforms4i;
    std::map<std::string, glm::mat3> m_instanceUniforms3m;
    std::map<std::string, glm::mat4> m_instanceUniforms4m;

    // 应用实例特定的统一值到 Shader 的方法
    void ApplyInstanceUniforms();
};

} // namespace Doodle