#pragma once

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
};

} // namespace Doodle