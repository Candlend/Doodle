#pragma once

#include "pch.h"

#include "ApplicationEvent.h"
#include "Camera.h"
#include "Component.h"
#include "Entity.h"
#include "EventManager.h"
#include "SceneRenderer.h"
#include "UniformBuffer.h"
#include <string>

namespace Doodle
{

class DOO_API Scene : public std::enable_shared_from_this<Scene>
{
    friend class SceneRenderer;

public:
    Scene();
    ~Scene();
    static std::shared_ptr<Scene> Create();
    std::shared_ptr<Entity> GetMainCameraEntity();

    std::shared_ptr<Entity> CreateEntity(const std::string &name);
    std::shared_ptr<Entity> FindEntity(const std::string &name) const
    {
        auto view = m_registry.view<TagComponent>();
        for (auto entity : view)
        {
            std::string tag = view.get<TagComponent>(entity);
            if (tag == name)
            {
                return std::make_shared<Entity>(m_registry, entity);
            }
        }
        return nullptr;
    }
    std::shared_ptr<Entity> GetEntity(const UUID &id) const;
    void AddEntity(const std::shared_ptr<Entity> &entity);
    void RemoveEntity(const UUID &id);
    void DestroyEntity(const std::shared_ptr<Entity> &entity);

    inline std::string GetName() const
    {
        return m_name;
    }

    inline bool IsActive() const
    {
        return m_active;
    }

    void BeginScene();
    void EndScene();

private:
    std::string m_name;
    bool m_active = false;
    std::unordered_map<UUID, entt::entity> m_entityMap;
    entt::registry m_registry;
    SceneRenderer m_sceneRenderer{shared_from_this()};
};

} // namespace Doodle
