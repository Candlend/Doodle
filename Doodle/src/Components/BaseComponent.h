#pragma once

#include "pch.h"
#include <entt/entt.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Entity.h"
#include "UUID.h"


namespace Doodle
{

class Scene;
class Scriptable;
struct DOO_API BaseComponent
{
    virtual ~BaseComponent()
    {
    }

    friend class Entity;
    operator Entity &();

    operator const Entity &() const;

    entt::entity GetEntityHandle() const;
    UUID GetUUID() const;
    Scene *GetScene() const;
    virtual const char *GetName() const = 0;

    virtual void OnInspectorLayout()
    {
        ImGui::TextDisabled("No inspector available");
    }

    virtual void OnDrawGizmos(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection)
    {
    }

    template <typename T, typename... Args> T &AddComponent(Args &&...args)
    {
        DOO_CORE_ASSERT(!HasComponent<T>(), "Entity already has component");
        static_assert(std::is_base_of_v<BaseComponent, T>, "T must derive from BaseComponent");
        T &comp = GetRegistry().emplace<T>(GetEntityHandle(), std::forward<Args>(args)...);
        comp.m_entity = *this;
        comp.m_entity.template OnComponentAdded<T>();
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

protected:
    Entity m_entity;
};

#define COMPONENT_CLASS_TYPE(type)                                                                                     \
    static const char *GetStaticName()                                                                                 \
    {                                                                                                                  \
        return #type;                                                                                                  \
    }                                                                                                                  \
    virtual const char *GetName() const override                                                                       \
    {                                                                                                                  \
        return GetStaticName();                                                                                        \
    }

} // namespace Doodle