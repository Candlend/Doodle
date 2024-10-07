#include "Material.h"
#include "Texture.h"
#include "TextureInfo.h"
#include "rfl/visit.hpp"

namespace Doodle
{

std::shared_ptr<Material> Material::Load(const std::filesystem::path &assetPath)
{
    auto asset = AssetManager::Get()->Create<MaterialAsset>(assetPath);
    return Create(asset->GetInfo());
}

std::shared_ptr<Material> Material::Create(const MaterialInfo &info)
{
    struct MaterialWrapper : public Material
    {
        MaterialWrapper(const MaterialInfo &info) : Material(info)
        {
        }
    };
    auto material = std::make_shared<MaterialWrapper>(info);
    ResourceManager::Get()->Add(info.UUID, material);
    return material;
}

void Material::Bind()
{
    m_shader->Bind();
    auto properties = m_info.Properties;
    for (auto &[name, value] : m_shaderProperties)
    {
        properties[name] = value;
    }

    for (auto &[name, value] : properties)
    {
        rfl::visit(
            [&](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int>)
                {
                    m_shader->SetUniform1i(name, arg);
                }
                else if constexpr (std::is_same_v<T, float>)
                {
                    m_shader->SetUniform1f(name, arg);
                }
                else if constexpr (std::is_same_v<T, glm::vec2>)
                {
                    m_shader->SetUniform2f(name, arg);
                }
                else if constexpr (std::is_same_v<T, glm::vec3>)
                {
                    m_shader->SetUniform3f(name, arg);
                }
                else if constexpr (std::is_same_v<T, glm::vec4>)
                {
                    m_shader->SetUniform4f(name, arg);
                }
                else if constexpr (std::is_same_v<T, glm::mat3>)
                {
                    m_shader->SetUniformMatrix3f(name, arg);
                }
                else if constexpr (std::is_same_v<T, glm::mat4>)
                {
                    m_shader->SetUniformMatrix4f(name, arg);
                }
                else if constexpr (std::is_same_v<T, glm::ivec2>)
                {
                    m_shader->SetUniform2i(name, arg);
                }
                else if constexpr (std::is_same_v<T, glm::ivec3>)
                {
                    m_shader->SetUniform3i(name, arg);
                }
                else if constexpr (std::is_same_v<T, glm::ivec4>)
                {
                    m_shader->SetUniform4i(name, arg);
                }
                else if constexpr (std::is_same_v<T, Texture2DInfo>)
                {
                    auto info = arg;
                    switch (info.Type)
                    {
                    case PrimitiveTextureType::White:
                        m_shader->SetUniformTexture(name, Texture2D::GetWhiteTexture());
                        break;
                    case PrimitiveTextureType::Black:
                        m_shader->SetUniformTexture(name, Texture2D::GetBlackTexture());
                        break;
                    case PrimitiveTextureType::Normal:
                        m_shader->SetUniformTexture(name, Texture2D::GetDefaultNormalTexture());
                        break;
                    default:
                        m_shader->SetUniformTexture(name, ResourceManager::Get()->Load<Texture2D>(info.UUID));
                        break;
                    }
                }
            },
            value);
    }
}

void Material::Unbind()
{
}

void Material::SetProperty(const std::string &name, const ShaderProperty &value)
{
    m_shaderProperties[name] = value;
}

ShaderProperty Material::GetProperty(const std::string &name) const
{
    return m_shaderProperties.at(name);
}

std::shared_ptr<Texture> Material::GetTexture(const std::string &name) const
{
    const auto *info = rfl::get_if<Texture2DInfo>(m_shaderProperties.at(name));
    if (info)
    {
        switch (info->Type)
        {
        case PrimitiveTextureType::White:
            return Texture2D::GetWhiteTexture();
        case PrimitiveTextureType::Black:
            return Texture2D::GetBlackTexture();
        case PrimitiveTextureType::Normal:
            return Texture2D::GetDefaultNormalTexture();
        default:
            return ResourceManager::Get()->Load<Texture2D>(info->UUID);
        }
    }

    // TODO TextureCube

    return nullptr;
}

} // namespace Doodle