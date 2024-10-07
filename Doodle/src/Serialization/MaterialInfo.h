#pragma once

#include "pch.h"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <rfl/Variant.hpp>

#include "TextureInfo.h"
#include "UUID.h"

namespace Doodle
{

using ShaderProperty = rfl::Variant<float, glm::vec2, glm::vec3, glm::vec4, glm::mat3, glm::mat4, int, glm::ivec2,
                                    glm::ivec3, glm::ivec4, bool, Texture2DInfo>;

struct MaterialInfo
{
    UUID UUID;
    std::string ShaderName;
    std::unordered_map<std::string, ShaderProperty> Properties;

    static MaterialInfo Standard()
    {
        MaterialInfo info;
        info.ShaderName = "standard";
        info.Properties["u_AlbedoColor"] = glm::vec4(1.0f);
        info.Properties["u_NormalScale"] = 1.0f;
        info.Properties["u_Metallic"] = 0.0f;
        info.Properties["u_Roughness"] = 0.5f;
        info.Properties["u_AlbedoTexture"] = Texture2DInfo(PrimitiveTextureType::White);
        info.Properties["u_NormalTexture"] = Texture2DInfo(PrimitiveTextureType::Normal);
        info.Properties["u_MetallicTexture"] = Texture2DInfo(PrimitiveTextureType::Black);
        info.Properties["u_RoughnessTexture"] = Texture2DInfo(PrimitiveTextureType::White);
        return info;
    }
};

} // namespace Doodle