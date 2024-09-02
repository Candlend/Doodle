#pragma once

#include "imgui.h"
#include "pch.h"

#include "BaseComponent.h"
#include "MaterialInstance.h"

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
        return "材质";
    }

    void OnInspectorLayout() override
    {
        ImGui::LabelText("Shader Path", "%s", MaterialInstance->GetMaterial()->GetShader()->GetPath().c_str());
        // TODO
    }
};

} // namespace Doodle