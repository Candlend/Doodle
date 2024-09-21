#include "BaseComponent.h"
#include "Entity.h"
#include "UUID.h"

namespace Doodle
{

BaseComponent::operator Entity &()
{
    return m_entity;
}

BaseComponent::operator const Entity &() const
{
    return m_entity;
}

entt::entity BaseComponent::GetEntityHandle() const
{
    if (!m_entity)
        return entt::null;
    return m_entity.GetEntityHandle();
}

UUID BaseComponent::GetUUID() const
{
    return m_entity.GetUUID();
}

Scene *BaseComponent::GetScene() const
{
    if (!m_entity)
        return nullptr;
    return m_entity.GetScene();
}

bool BaseComponent::IsValid() const
{
    return m_entity;
}

BaseComponent::operator bool() const
{
    return IsValid();
}

entt::registry &BaseComponent::GetRegistry() const
{
    return m_entity.GetRegistry();
}

Entity BaseComponent::GetParent() const
{
    return m_entity.GetParent();
}

void BaseComponent::SetParent(Entity parent)
{
    m_entity.SetParent(parent);
}

void BaseComponent::SetParentUUID(UUID parent)
{
    m_entity.SetParentUUID(parent);
}

UUID BaseComponent::GetParentUUID() const
{
    return m_entity.GetParentUUID();
}

std::vector<Entity> BaseComponent::GetChildren()
{
    return m_entity.GetChildren();
}

std::vector<Entity> BaseComponent::GetChildren() const
{
    return m_entity.GetChildren();
}

std::vector<UUID> &BaseComponent::GetChildrenUUID()
{
    return m_entity.GetChildrenUUID();
}

const std::vector<UUID> &BaseComponent::GetChildrenUUID() const
{
    return m_entity.GetChildrenUUID();
}

bool BaseComponent::RemoveChild(Entity child)
{
    return m_entity.RemoveChild(child);
}

bool BaseComponent::IsAncestorOf(Entity entity) const
{
    return m_entity.IsAncestorOf(entity);
}

bool BaseComponent::IsDescendantOf(Entity entity) const
{
    return m_entity.IsDescendantOf(entity);
}

} // namespace Doodle