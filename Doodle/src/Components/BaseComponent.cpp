#include "BaseComponent.h"
#include "Entity.h"

namespace Doodle
{

BaseComponent::operator Entity &()
{
    return *m_entity;
}

BaseComponent::operator const Entity &() const
{
    return *m_entity;
}

entt::entity BaseComponent::GetEntityHandle() const
{
    if (!m_entity)
        return entt::null;
    return m_entity->GetEntityHandle();
}

UUID BaseComponent::GetUUID() const
{
    return m_entity->GetUUID();
}

Scene *BaseComponent::GetScene() const
{
    if (!m_entity)
        return nullptr;
    return m_entity->GetScene();
}

bool BaseComponent::IsValid() const
{
    return m_entity && m_entity->IsValid();
}

BaseComponent::operator bool() const
{
    return IsValid();
}

entt::registry &BaseComponent::GetRegistry() const
{
    return m_entity->GetRegistry();
}
} // namespace Doodle