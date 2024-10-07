#pragma once

#include "pch.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <memory>
#include <rfl/Variant.hpp>

#include "AssetManager.h"
#include "MaterialAsset.h"
#include "MaterialInfo.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "ShaderLibrary.h"
#include "Texture.h"

namespace Doodle
{

class DOO_API Material
{
public:
    static std::shared_ptr<Material> Load(const std::filesystem::path &assetPath);

    static std::shared_ptr<Material> Create(const MaterialInfo &info = MaterialInfo());

    virtual void Bind();
    virtual void Unbind();

    void SetProperty(const std::string &name, const ShaderProperty &value);
    ShaderProperty GetProperty(const std::string &name) const;
    template <typename T> T GetProperty(const std::string &name) const
    {
        return rfl::get<T>(GetProperty(name));
    }

    std::shared_ptr<Shader> GetShader() const
    {
        return m_shader;
    }

    std::string GetShaderName() const
    {
        return m_info.ShaderName;
    }

    std::shared_ptr<Texture> GetTexture(const std::string &name) const;

protected:
    Material() = default;

    Material(const MaterialInfo &info) : m_info(info), m_shader(ShaderLibrary::Get()->GetShader(m_info.ShaderName))
    {
    }
    MaterialInfo m_info;
    std::shared_ptr<Shader> m_shader;
    std::unordered_map<std::string, ShaderProperty> m_shaderProperties;
};

} // namespace Doodle
