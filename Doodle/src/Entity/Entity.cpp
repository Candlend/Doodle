#include "Entity.h"
#include "Component.h"
#include "Scene.h"

namespace Doodle
{

Entity Entity::Create(Scene *scene)
{
    return Entity(scene, scene->m_registry.create());
}

Entity::Entity() : m_scene(nullptr), m_entityHandle(entt::null)
{
}

Entity::Entity(Scene *scene, entt::entity id) : m_scene(scene), m_entityHandle(id)
{
}

Entity::Entity(const Scene *scene, entt::entity id) : m_scene(const_cast<Scene *>(scene)), m_entityHandle(id)
{
}

entt::entity Entity::GetEntityHandle() const
{
    return m_entityHandle;
}

UUID Entity::GetUUID() const
{
    return GetComponent<IDComponent>();
}

Scene *Entity::GetScene() const
{
    return m_scene;
}

bool Entity::IsValid() const
{
    return (m_entityHandle != entt::null) && m_scene && GetRegistry().valid(m_entityHandle);
}

Entity::operator bool() const
{
    return IsValid();
}

entt::registry &Entity::GetRegistry() const
{
    return m_scene->m_registry;
}

Entity Entity::GetParent() const
{
    auto parentUUID = GetParentUUID();
    if (parentUUID == UUID::Nil())
        return Entity();
    return m_scene->GetEntity(parentUUID);
}

void Entity::SetParent(Entity parent)
{
    Entity currentParent = GetParent();
    if (currentParent == parent)
        return;

    // If changing parent, remove child from existing parent
    if (currentParent)
        currentParent.RemoveChild(*this);

    // Setting to null is okay
    SetParentUUID(parent.GetUUID());

    if (parent)
    {
        auto &parentChildren = parent.GetChildrenUUID();
        UUID uuid = GetUUID();
        if (std::find(parentChildren.begin(), parentChildren.end(), uuid) == parentChildren.end())
            parentChildren.emplace_back(GetUUID());
    }
}

void Entity::SetParentUUID(UUID parent)
{
    GetComponent<TransformComponent>().ParentHandle = parent;
}

UUID Entity::GetParentUUID() const
{
    return GetComponent<TransformComponent>().ParentHandle;
}

std::vector<Entity> Entity::GetChildren()
{
    std::vector<Entity> children;
    for (UUID child : GetChildrenUUID())
    {
        children.emplace_back(m_scene->GetEntity(child));
    }
    return children;
}

std::vector<Entity> Entity::GetChildren() const
{
    std::vector<Entity> children;
    for (UUID child : GetChildrenUUID())
    {
        children.emplace_back(m_scene->GetEntity(child));
    }
    return children;
}

std::vector<UUID> &Entity::GetChildrenUUID()
{
    return GetComponent<TransformComponent>().Children;
}

const std::vector<UUID> &Entity::GetChildrenUUID() const
{
    return GetComponent<TransformComponent>().Children;
}

bool Entity::RemoveChild(Entity child)
{
    UUID childId = child.GetUUID();
    std::vector<UUID> &children = GetChildrenUUID();
    auto it = std::find(children.begin(), children.end(), childId);
    if (it != children.end())
    {
        children.erase(it);
        GetComponent<TransformComponent>().Children = children;
        return true;
    }

    return false;
}

bool Entity::IsAncestorOf(Entity entity) const
{
    const auto &children = GetChildrenUUID();

    if (children.empty())
        return false;

    for (UUID child : children)
    {
        if (child == entity.GetUUID())
            return true;
    }

    for (UUID child : children)
    {
        if (m_scene->GetEntity(child).IsAncestorOf(entity))
            return true;
    }

    return false;
}

bool Entity::IsDescendantOf(Entity entity) const
{
    return entity.IsAncestorOf(*this);
}

} // namespace Doodle