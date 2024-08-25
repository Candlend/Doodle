#pragma once

#include "pch.h"
#include <entt/entt.hpp>
#include <memory>

#include "Component.h"

namespace Doodle
{

class Scene;
class DOO_API Entity
{
public:
    static std::shared_ptr<Entity> Create(entt::registry &registry);
    explicit Entity(entt::registry &registry, entt::entity id);
    explicit Entity(entt::registry &registry);
    entt::entity GetId() const;

    template <typename T, typename... Args> T &AddComponent(Args &&...args)
    {
        DOO_CORE_ASSERT(!HasComponent<T>(), "Entity already has component");
        return m_registry.emplace<T>(m_id, std::forward<Args>(args)...);
    }

    template <typename T> T &GetComponent()
    {
        DOO_CORE_ASSERT(HasComponent<T>(), "Entity does not have component");
        return m_registry.get<T>(m_id);
    }

    template <typename T> const T &GetComponent() const
    {
        DOO_CORE_ASSERT(HasComponent<T>(), "Entity does not have component");
        return m_registry.get<T>(m_id);
    }

    template <typename T> T *TryGetComponent()
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        return m_registry.try_get<T>(m_id);
    }

    template <typename T> const T *TryGetComponent() const
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        return m_registry.try_get<T>(m_id);
    }

    template <typename... T> bool HasComponent()
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        return m_registry.all_of<T...>(m_id);
    }

    template <typename... T> bool HasComponent() const
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        return m_registry.all_of<T...>(m_id);
    }

    template <typename... T> bool HasAny()
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        return m_registry.any_of<T...>(m_id);
    }

    template <typename... T> bool HasAny() const
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        return m_registry.any_of<T...>(m_id);
    }

    template <typename T> void RemoveComponent()
    {
        DOO_CORE_ASSERT(HasComponent<T>(), "Entity does not have component");
        m_registry.remove<T>(m_id);
    }

    template <typename T> void RemoveComponentIfExists()
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        if (HasComponent<T>())
        {
            m_registry.remove<T>(m_id);
        }
    }

    bool IsValid() const;
    explicit operator bool() const;

private:
    entt::registry &m_registry;
    entt::entity m_id;
};

} // namespace Doodle