#pragma once

#include "pch.h"
#include <entt/entt.hpp>

#include "Camera.h"
#include "Component.h"
#include "Entity.h"

namespace Doodle
{

class DOO_API Scene : public std::enable_shared_from_this<Scene>
{
public:
    static std::shared_ptr<Scene> Create();
    std::shared_ptr<Entity> CreateEntity(const std::string &name);
    std::shared_ptr<Entity> GetEntity(const std::string &name) const;
    void Render();
    std::shared_ptr<Entity> GetMainCameraEntity();
    void Begin();
    void End();

private:
    entt::registry m_registry;
    std::unordered_map<std::string, std::shared_ptr<Entity>> m_entities;
};

} // namespace Doodle
