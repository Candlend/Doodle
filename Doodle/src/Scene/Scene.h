#pragma once

#include "pch.h"

#include <entt/entt.hpp>
#include <unordered_map>

#include "Camera.h"
#include "Component.h"
#include "Entity.h"

namespace Doodle
{

class DOO_API Scene
{
public:
    static std::shared_ptr<Scene> Create();
    std::shared_ptr<Entity> CreateEntity(const std::string &name);
    std::shared_ptr<Entity> GetEntity(const std::string &name) const;
    void Render(const std::shared_ptr<Camera> &camera);

private:
    entt::registry m_registry;
    std::unordered_map<std::string, std::shared_ptr<Entity>> m_entities;
};

} // namespace Doodle