#pragma once

#include "Log.h"
#include "glm/fwd.hpp"
#include "pch.h"
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Entity.h"
#include "UUID.h"


namespace Doodle
{

class Entity;
class Scene;
class Scriptable;
struct DOO_API BaseComponent
{
    BaseComponent() = default;

    virtual ~BaseComponent()
    {
    }

    friend class Entity;
    operator Entity &();

    operator const Entity &() const;

    entt::entity GetEntityHandle() const;
    UUID GetUUID() const;
    Scene *GetScene() const;

    template <typename T, typename... Args> T &AddComponent(Args &&...args)
    {
        DOO_CORE_ASSERT(!HasComponent<T>(), "Entity already has component");
        static_assert(std::is_base_of_v<BaseComponent, T>, "T must derive from BaseComponent");
        T &comp = GetRegistry().emplace<T>(GetEntityHandle(), std::forward<Args>(args)...);
        comp.m_entity = *this;
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
        GetRegistry().template remove<T>(GetEntityHandle());
    }

    template <typename T> void RemoveComponentIfExists()
    {
        DOO_CORE_ASSERT(IsValid(), "Entity is not valid");
        if (HasComponent<T>())
        {
            GetRegistry().template remove<T>(GetEntityHandle());
        }
    }

    bool IsValid() const;
    operator bool() const;

protected:
    entt::registry &GetRegistry() const;
    Entity m_entity;
};

} // namespace Doodle