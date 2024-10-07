#pragma once

#include "pch.h"
#include <entt/entt.hpp>

#include "UUID.h"

namespace Doodle
{
class BaseComponent;
class Scriptable;
class Scene;
class DOO_API Entity
{
public:
    static Entity Create(Scene *scene);
    Entity(); // ? 是否该设成private
    ~Entity()
    {
    }

    Entity(const Entity &other) = default;
    Entity(Entity &&other) = default;
    Entity &operator=(const Entity &other) = default;
    Entity &operator=(Entity &&other) = default;

    Entity(Scene *scene, entt::entity id);
    Entity(const Scene *scene, entt::entity id);

    entt::entity GetEntityHandle() const;
    UUID GetUUID() const;
    std::string GetName() const;
    Scene *GetScene() const;
    std::vector<BaseComponent *> &GetComponents() const;

    template <typename T, typename... Args> T &AddComponent(Args &&...args)
    {
        DOO_CORE_ASSERT(!HasComponent<T>(), "Entity already has component");
        static_assert(std::is_base_of_v<BaseComponent, T>, "T must derive from BaseComponent");
        T &comp = GetRegistry().emplace<T>(GetEntityHandle(), std::forward<Args>(args)...);
        comp.m_entity = *this;
        comp.m_entity.OnComponentAdded(&GetComponent<T>());
        if constexpr (std::is_base_of_v<Scriptable, T>)
        {
            comp.OnAdded();
        }
        return comp;
    }

    template <typename T> T &GetComponent()
    {
        DOO_CORE_ASSERT(HasComponent<T>(), "Entity does not have component");
        return GetRegistry().template get<T>(GetEntityHandle());
    }

    template <typename T> const T &GetComponent() const
    {
        DOO_CORE_ASSERT(HasComponent<T>(), "Entity does not have component");
        return GetRegistry().template get<T>(GetEntityHandle());
    }

    template <typename T> T *TryGetComponent()
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        return GetRegistry().template try_get<T>(GetEntityHandle());
    }

    template <typename T> const T *TryGetComponent() const
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        return GetRegistry().template try_get<T>(GetEntityHandle());
    }

    template <typename... T> bool HasComponent()
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        return GetRegistry().template all_of<T...>(GetEntityHandle());
    }

    template <typename... T> bool HasComponent() const
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        return GetRegistry().template all_of<T...>(GetEntityHandle());
    }

    template <typename... T> bool HasAny()
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        return GetRegistry().template any_of<T...>(GetEntityHandle());
    }

    template <typename... T> bool HasAny() const
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        return GetRegistry().template any_of<T...>(GetEntityHandle());
    }

    template <typename T> void RemoveComponent()
    {
        DOO_CORE_ASSERT(HasComponent<T>(), "Entity does not have component");
        T &comp = GetComponent<T>();
        comp.m_entity.OnComponentRemoved(&GetComponent<T>());
        GetRegistry().template remove<T>(GetEntityHandle());
    }

    template <typename T> void RemoveComponentIfExists()
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        if (HasComponent<T>())
        {
            T &comp = GetComponent<T>();
            comp.m_entity.OnComponentRemoved(&GetComponent<T>());
            GetRegistry().template remove<T>(GetEntityHandle());
        }
    }

    bool IsValid() const;
    operator bool() const;

    bool operator==(const Entity &other) const
    {
        return m_entityHandle == other.m_entityHandle;
    }

    entt::registry &GetRegistry() const;

    Entity GetParent() const;

    void SetParent(Entity parent);

    void SetParentUUID(UUID parent);
    UUID GetParentUUID() const;
    std::vector<Entity> GetChildren();
    std::vector<Entity> GetChildren() const;
    std::vector<UUID> &GetChildrenUUID();
    const std::vector<UUID> &GetChildrenUUID() const;

    bool RemoveChild(Entity child);

    bool IsAncestorOf(Entity entity) const;
    bool IsDescendantOf(Entity entity) const;

private:
    void OnComponentAdded(BaseComponent *component);
    void OnComponentRemoved(BaseComponent *component);
    Scene *m_scene;
    entt::entity m_entityHandle;
};

} // namespace Doodle