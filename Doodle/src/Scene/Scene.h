#pragma once

#include "pch.h"
#include <entt/entt.hpp>

#include "ApplicationEvent.h"
#include "Camera.h"
#include "EventManager.h"
#include "UUID.h"
#include "UniformBuffer.h"

namespace Doodle
{

class Entity;
class BaseComponent;
class SceneRenderer;
class DOO_API Scene : public std::enable_shared_from_this<Scene>
{
    friend class SceneRenderer;
    friend class Entity;

public:
    static std::shared_ptr<Scene> Create(const std::string &name);
    Scene(const std::string &name);
    ~Scene();
    Entity GetMainCameraEntity();

    Entity CreateEntity(const std::string &name);
    Entity FindEntity(const std::string &name) const;
    Entity GetEntity(const UUID &id) const;
    std::vector<Entity> GetEntities() const;
    std::vector<BaseComponent *> GetComponents(const UUID &id) const;
    void AddEntity(const Entity &entity);
    void RemoveEntity(const UUID &id);
    void DestroyEntity(const Entity &entity);

    inline std::string GetName() const
    {
        return m_name;
    }

    inline bool IsActive() const
    {
        return m_active;
    }

    bool operator==(const Scene &other) const
    {
        return m_name == other.m_name;
    }

    void BeginScene();
    void EndScene();

private:
    std::string m_name;
    bool m_active = false;
    std::unordered_map<UUID, Entity> m_entityMap;
    std::unordered_map<UUID, std::vector<BaseComponent *>> m_entityComponents;
    entt::registry m_registry;
};

} // namespace Doodle
