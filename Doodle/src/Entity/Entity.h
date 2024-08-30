#pragma once

#include "BaseComponent.h"
#include "UUID.h"
#include "pch.h"
#include <entt/entt.hpp>


namespace Doodle
{

class Scriptable;
class Scene;
class DOO_API Entity
{
public:
    static std::shared_ptr<Entity> Create(Scene *scene);
    Entity(Scene *scene, entt::entity id);
    Entity(const Scene *scene, entt::entity id);

    entt::entity GetEntityHandle() const;
    UUID GetUUID() const;
    Scene *GetScene() const;

    template <typename T, typename... Args> T &AddComponent(Args &&...args)
    {
        DOO_CORE_ASSERT(!HasComponent<T>(), "Entity already has component");
        static_assert(std::is_base_of_v<BaseComponent, T>, "T must derive from BaseComponent");
        return GetRegistry().emplace<T>(m_entityHandle, std::forward<Args>(args)...);
    }

    template <typename T> T &GetComponent()
    {
        DOO_CORE_ASSERT(HasComponent<T>(), "Entity does not have component");
        return GetRegistry().get<T>(m_entityHandle);
    }

    template <typename T> const T &GetComponent() const
    {
        DOO_CORE_ASSERT(HasComponent<T>(), "Entity does not have component");
        return GetRegistry().get<T>(m_entityHandle);
    }

    template <typename T> T *TryGetComponent()
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        return GetRegistry().try_get<T>(m_entityHandle);
    }

    template <typename T> const T *TryGetComponent() const
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        return GetRegistry().try_get<T>(m_entityHandle);
    }

    template <typename... T> bool HasComponent()
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        return GetRegistry().all_of<T...>(m_entityHandle);
    }

    template <typename... T> bool HasComponent() const
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        return GetRegistry().all_of<T...>(m_entityHandle);
    }

    template <typename... T> bool HasAny()
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        return GetRegistry().any_of<T...>(m_entityHandle);
    }

    template <typename... T> bool HasAny() const
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        return GetRegistry().any_of<T...>(m_entityHandle);
    }

    template <typename T> void RemoveComponent()
    {
        DOO_CORE_ASSERT(HasComponent<T>(), "Entity does not have component");
        GetRegistry().remove<T>(m_entityHandle);
    }

    template <typename T> void RemoveComponentIfExists()
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        if (HasComponent<T>())
        {
            GetRegistry().remove<T>(m_entityHandle);
        }
    }

    bool IsValid() const;
    operator bool() const;

    bool operator==(const Entity &other) const
    {
        return m_entityHandle == other.m_entityHandle;
    }

    entt::registry &GetRegistry() const;

private:
    Scene *m_scene;
    entt::entity m_entityHandle;
};

} // namespace Doodle