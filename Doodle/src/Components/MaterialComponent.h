#pragma once

#include "pch.h"
#include <imgui.h>

#include "BaseComponent.h"
#include "ImGuiUtils.h"
#include "Material.h"
#include "MaterialInfo.h"
#include "Shader.h"
#include "Texture.h"


namespace Doodle
{

struct MaterialComponent : public BaseComponent
{
    COMPONENT_CLASS_TYPE(Material)

    std::shared_ptr<Material> Material;
    MaterialInfo Info;

    MaterialComponent(const MaterialInfo &info = MaterialInfo::Standard())
        : Info(info), Material(Material::Create(info))
    {
    }

    operator Doodle::Material &() const
    {
        return *Material;
    }

    operator const Doodle::Material &() const
    {
        return *Material;
    }

    void OnInspectorLayout() override;

    rfl::Generic::Object SerializeToObject() const override;

    void DeserializeFromObject(const rfl::Generic::Object &object) override;

    void Reload();
};

} // namespace Doodle
