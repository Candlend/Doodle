#pragma once

#include "pch.h"

#include <entt/entt.hpp>

#include "Camera.h"
#include "Component.h"
#include "Entity.h"

namespace Doodle
{

class DOO_API Scene
{
public:
    static std::shared_ptr<Scene> Create();
    Entity CreateEntity();
    void Render() const;

private:
    entt::registry m_registry;
};

} // namespace Doodle