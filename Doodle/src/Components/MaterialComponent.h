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

    std::string GetName() const override
    {
        return "Material";
    }

    void OnInspectorLayout() override
    {
        ImGuiUtils::ReadOnlyInputText("Shader Path", MaterialInstance->GetShader()->GetPath());
        // TODO
    }
};

} // namespace Doodle