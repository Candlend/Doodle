#pragma once

#include "imgui.h"
#include "pch.h"

#include "BaseComponent.h"
#include "MaterialInstance.h"
#include "ImGuiUtils.h"

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
        // TODO
    }
};

} // namespace Doodle