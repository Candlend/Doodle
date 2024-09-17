#pragma once

#include "pch.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>

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

    Material(std::shared_ptr<Shader> shader) : m_shader(shader)
    {
    }

    Material(const std::string &shaderName)
    {
        m_shaderName = shaderName;
        m_shader = ShaderLibrary::Get()->GetShader(shaderName);
    }

    virtual void Bind();
    virtual void Unbind();

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
    void SetUniformTexture(const std::string &name, uint64_t textureHandle);

    float GetUniform1f(const std::string &name);
    glm::vec2 GetUniform2f(const std::string &name);
    glm::vec3 GetUniform3f(const std::string &name);
    glm::vec4 GetUniform4f(const std::string &name);
    int GetUniform1i(const std::string &name);
    glm::ivec2 GetUniform2i(const std::string &name);
    glm::ivec3 GetUniform3i(const std::string &name);
    glm::ivec4 GetUniform4i(const std::string &name);
    glm::mat3 GetUniformMatrix3f(const std::string &name);
    glm::mat4 GetUniformMatrix4f(const std::string &name);
    std::shared_ptr<Texture> GetUniformTexture(const std::string &name);
    uint64_t GetUniformTextureHandle(const std::string &name);

    std::shared_ptr<Shader> GetShader() const
    {
        return m_shader;
    }

    std::string GetShaderName() const
    {
        return m_shaderName;
    }

    std::vector<ShaderProperty> GetProperties() const
    {
        return m_shader->GetProperties();
    }

private:
    std::string m_shaderName;
    std::shared_ptr<Shader> m_shader;
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
    std::unordered_map<std::string, uint64_t> m_textureHandles;
    std::unordered_map<std::string, float> m_uniforms1f;
    std::unordered_map<std::string, glm::vec2> m_uniforms2f;
    std::unordered_map<std::string, glm::vec3> m_uniforms3f;
    std::unordered_map<std::string, glm::vec4> m_uniforms4f;
    std::unordered_map<std::string, float> m_uniforms1i;
    std::unordered_map<std::string, glm::ivec2> m_uniforms2i;
    std::unordered_map<std::string, glm::ivec3> m_uniforms3i;
    std::unordered_map<std::string, glm::ivec4> m_uniforms4i;
    std::unordered_map<std::string, glm::mat3> m_uniforms3m;
    std::unordered_map<std::string, glm::mat4> m_uniforms4m;
};

class DOO_API StandardMaterial : public Material
{
public:
    static std::shared_ptr<StandardMaterial> Create()
    {
        return std::make_shared<StandardMaterial>();
    }

    StandardMaterial() : Material("standard")
    {
        SetAlbedoTexture(Texture2D::GetWhiteTexture());
        SetNormalTexture(Texture2D::GetDefaultNormalTexture());
        SetMetallicTexture(Texture2D::GetWhiteTexture());
        SetRoughnessTexture(Texture2D::GetWhiteTexture());
        SetAlbedoColor(glm::vec4(1.0f));
        SetNormalScale(1.0f);
        SetMetallic(0.5f);
        SetRoughness(0.5f);
    }

    void SetAlbedoColor(const glm::vec4 &color)
    {
        SetUniform4f("u_AlbedoColor", color);
    }

    void SetNormalScale(float scale)
    {
        SetUniform1f("u_NormalScale", scale);
    }

    void SetMetallic(float metallic)
    {
        SetUniform1f("u_Metallic", metallic);
    }

    void SetRoughness(float roughness)
    {
        SetUniform1f("u_Roughness", roughness);
    }

    void SetAlbedoTexture(std::shared_ptr<Texture> albedoTexture)
    {
        SetUniformTexture("u_AlbedoTexture", albedoTexture);
    }

    void SetNormalTexture(std::shared_ptr<Texture> normalTexture)
    {
        SetUniformTexture("u_NormalTexture", normalTexture);
    }

    void SetMetallicTexture(std::shared_ptr<Texture> metallicTexture)
    {
        SetUniformTexture("u_MetallicTexture", metallicTexture);
    }

    void SetRoughnessTexture(std::shared_ptr<Texture> roughnessTexture)
    {
        SetUniformTexture("u_RoughnessTexture", roughnessTexture);
    }

    void LoadAlbedoTexture(const std::string &filepath, const TextureParams &params = TextureParams())
    {
        SetAlbedoTexture(Texture2D::Create(filepath, params));
    }

    void LoadNormalTexture(const std::string &filepath, const TextureParams &params = TextureParams())
    {
        SetNormalTexture(Texture2D::Create(filepath, params));
    }

    void LoadMetallicTexture(const std::string &filepath, const TextureParams &params = TextureParams())
    {
        SetMetallicTexture(Texture2D::Create(filepath, params));
    }

    void LoadRoughnessTexture(const std::string &filepath, const TextureParams &params = TextureParams())
    {
        SetRoughnessTexture(Texture2D::Create(filepath, params));
    }

    void LoadAlbedoTexture(Buffer buffer, const TextureParams &params = TextureParams())
    {
        SetAlbedoTexture(Texture2D::Create(buffer, params));
    }

    void LoadNormalTexture(Buffer buffer, const TextureParams &params = TextureParams())
    {
        SetNormalTexture(Texture2D::Create(buffer, params));
    }

    void LoadMetallicTexture(Buffer buffer, const TextureParams &params = TextureParams())
    {
        SetMetallicTexture(Texture2D::Create(buffer, params));
    }

    void LoadRoughnessTexture(Buffer buffer, const TextureParams &params = TextureParams())
    {
        SetRoughnessTexture(Texture2D::Create(buffer, params));
    }
};

} // namespace Doodle
