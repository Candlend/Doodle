#pragma once

#include "UUID.h"
#include "pch.h"
#include <entt/entt.hpp>


namespace Doodle
{

class Scene;
class DOO_API Entity
{
public:
    static std::shared_ptr<Entity> Create(entt::registry &registry);
    Entity(entt::registry &registry, entt::entity id);
    Entity(const entt::registry &registry, entt::entity id);
    Entity(entt::registry &registry);
    entt::entity GetEntityHandle() const;
    UUID GetUUID() const;

    template <typename T, typename... Args> T &AddComponent(Args &&...args)
    {
        DOO_CORE_ASSERT(!HasComponent<T>(), "Entity already has component");
        return m_registry.emplace<T>(m_entityHandle, std::forward<Args>(args)...);
    }

    template <typename T> T &GetComponent()
    {
        DOO_CORE_ASSERT(HasComponent<T>(), "Entity does not have component");
        return m_registry.get<T>(m_entityHandle);
    }

    template <typename T> const T &GetComponent() const
    {
        DOO_CORE_ASSERT(HasComponent<T>(), "Entity does not have component");
        return m_registry.get<T>(m_entityHandle);
    }

    template <typename T> T *TryGetComponent()
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        return m_registry.try_get<T>(m_entityHandle);
    }

    template <typename T> const T *TryGetComponent() const
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        return m_registry.try_get<T>(m_entityHandle);
    }

    template <typename... T> bool HasComponent()
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        return m_registry.all_of<T...>(m_entityHandle);
    }

    template <typename... T> bool HasComponent() const
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        return m_registry.all_of<T...>(m_entityHandle);
    }

    template <typename... T> bool HasAny()
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        return m_registry.any_of<T...>(m_entityHandle);
    }

    template <typename... T> bool HasAny() const
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        return m_registry.any_of<T...>(m_entityHandle);
    }

    template <typename T> void RemoveComponent()
    {
        DOO_CORE_ASSERT(HasComponent<T>(), "Entity does not have component");
        m_registry.remove<T>(m_entityHandle);
    }

    template <typename T> void RemoveComponentIfExists()
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        if (HasComponent<T>())
        {
            m_registry.remove<T>(m_entityHandle);
        }
    }

    bool IsValid() const;
    operator bool() const;

private:
    entt::registry &m_registry;
    entt::entity m_entityHandle;
};

} // namespace Doodle