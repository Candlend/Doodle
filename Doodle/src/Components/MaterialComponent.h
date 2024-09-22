#pragma once

#include "Log.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "imgui.h"
#include "pch.h"

#include "BaseComponent.h"
#include "ImGuiUtils.h"
#include "MaterialInstance.h"
#include <vector>

namespace Doodle
{

struct MaterialComponent : public BaseComponent
{
    COMPONENT_CLASS_TYPE(Material)

    std::shared_ptr<MaterialInstance> MaterialInstance;

    MaterialComponent(const std::shared_ptr<Doodle::MaterialInstance> &materialInstance)
        : MaterialInstance(materialInstance)
    {
    }

    MaterialComponent(const std::shared_ptr<Material> &material) : MaterialInstance(MaterialInstance::Create(material))
    {
    }

    operator Doodle::MaterialInstance &() const
    {
        return *MaterialInstance;
    }

    operator const Doodle::MaterialInstance &() const
    {
        return *MaterialInstance;
    }

    void OnInspectorLayout() override
    {
        ImGuiUtils::ReadOnlyInputText("Shader Path", MaterialInstance->GetShader()->GetPath());
        std::shared_ptr<Material> material = MaterialInstance->GetMaterial();
        auto standardMaterial = dynamic_pointer_cast<StandardMaterial>(material);
        if (!standardMaterial)
        {
            DOO_CORE_WARN("Now only support StandardMaterial");
            return;
        }

        std::vector<ShaderProperty> properties = MaterialInstance->GetProperties();

        for (const auto &property : properties)
        {
            std::string propertyName = property.Name + "##" + GetUUID().ToString();
            switch (property.Type)
            {
            case ShaderPropertyType::None:
                break;
            case ShaderPropertyType::Float: {
                float value = MaterialInstance->GetUniform1f(property.Name);
                if (ImGui::DragFloat(propertyName.c_str(), &value, 0.01f))
                {
                    MaterialInstance->SetUniform1f(property.Name, value);
                }
            }
            break;
            case ShaderPropertyType::Float2: {
                glm::vec2 value = MaterialInstance->GetUniform2f(property.Name);
                if (ImGui::DragFloat2(propertyName.c_str(), &value.x, 0.01f))
                {
                    MaterialInstance->SetUniform2f(property.Name, value);
                }
            }
            break;
            case ShaderPropertyType::Float3: {
                glm::vec3 value = MaterialInstance->GetUniform3f(property.Name);
                if (property.Name.find("Color") != std::string::npos)
                {
                    if (ImGui::ColorEdit3(propertyName.c_str(), &value.x))
                    {
                        MaterialInstance->SetUniform3f(property.Name, value);
                    }
                }
                else if (ImGui::DragFloat3(propertyName.c_str(), &value.x, 0.01f))
                {
                    MaterialInstance->SetUniform3f(property.Name, value);
                }
            }
            break;
            case ShaderPropertyType::Float4: {
                glm::vec4 value = MaterialInstance->GetUniform4f(property.Name);
                if (property.Name.find("Color") != std::string::npos)
                {
                    if (ImGui::ColorEdit4(propertyName.c_str(), &value.x))
                    {
                        MaterialInstance->SetUniform4f(property.Name, value);
                    }
                }
                else if (ImGui::DragFloat4(propertyName.c_str(), &value.x, 0.01f))
                {
                    MaterialInstance->SetUniform4f(property.Name, value);
                }
            }
            break;
            case ShaderPropertyType::Mat3:
                break;
            case ShaderPropertyType::Mat4:
                break;
            case ShaderPropertyType::Int: {
                int value = MaterialInstance->GetUniform1i(property.Name);
                if (ImGui::DragInt(propertyName.c_str(), &value))
                {
                    MaterialInstance->SetUniform1i(property.Name, value);
                }
            }
            break;
            case ShaderPropertyType::Int2: {
                glm::ivec2 value = MaterialInstance->GetUniform2i(property.Name);
                if (ImGui::DragInt2(propertyName.c_str(), &value.x))
                {
                    MaterialInstance->SetUniform2i(property.Name, value);
                }
            }
            break;
            case ShaderPropertyType::Int3: {
                glm::ivec3 value = MaterialInstance->GetUniform3i(property.Name);
                if (ImGui::DragInt3(propertyName.c_str(), &value.x))
                {
                    MaterialInstance->SetUniform3i(property.Name, value);
                }
            }
            break;
            case ShaderPropertyType::Int4: {
                glm::ivec4 value = MaterialInstance->GetUniform4i(property.Name);
                if (ImGui::DragInt4(propertyName.c_str(), &value.x))
                {
                    MaterialInstance->SetUniform4i(property.Name, value);
                }
            }
            break;
            case ShaderPropertyType::Bool: {
                int value = MaterialInstance->GetUniform1i(property.Name);
                if (ImGui::Checkbox(propertyName.c_str(), reinterpret_cast<bool *>(&value)))
                {
                    MaterialInstance->SetUniform1i(property.Name, value);
                }
            }
            break;
            case ShaderPropertyType::Sampler2D: {
                auto texture = MaterialInstance->GetUniformTexture(property.Name);
                if (texture)
                {
                    ImGui::Image(reinterpret_cast<void *>(static_cast<intptr_t>(texture->GetRendererID())),
                                 ImVec2(64, 64));
                    ImGui::SameLine();
                    ImGui::Text("%s", property.Name.c_str());
                }
                break;
            }
            case ShaderPropertyType::SamplerCube:
                break;
            }
        }
    }
};

} // namespace Doodle