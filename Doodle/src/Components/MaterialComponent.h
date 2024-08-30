#pragma once

#include "MaterialInstance.h"

namespace Doodle
{

struct MaterialComponent
{
    std::shared_ptr<MaterialInstance> MaterialInstance;

    MaterialComponent(const std::shared_ptr<Doodle::MaterialInstance> &materialInstance)
        : MaterialInstance(materialInstance)
    {
    }

    MaterialComponent(const std::shared_ptr<Material> &material) : MaterialInstance(MaterialInstance::Create(material))
    {
    }

    operator std::shared_ptr<Doodle::MaterialInstance> &()
    {
        return MaterialInstance;
    }

    operator const std::shared_ptr<Doodle::MaterialInstance> &() const
    {
        return MaterialInstance;
    }
};

} // namespace Doodle