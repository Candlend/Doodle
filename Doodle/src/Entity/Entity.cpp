#include "Entity.h"

#include "Component.h"
#include "Scene.h"

namespace Doodle
{

std::shared_ptr<Entity> Entity::Create(entt::registry &registry)
{
    return std::make_shared<Entity>(registry);
}

Entity::Entity(entt::registry &registry, entt::entity id) : m_registry(registry), m_entityHandle(id)
{
}

Entity::Entity(const entt::registry &registry, entt::entity id)
    : m_registry(const_cast<entt::registry &>(registry)), m_entityHandle(id)
{
}

Entity::Entity(entt::registry &registry) : m_registry(registry)
{
    m_entityHandle = m_registry.create();
}

entt::entity Entity::GetEntityHandle() const
{
    return m_entityHandle;
}

UUID Entity::GetUUID() const
{
    return GetComponent<IDComponent>();
}

bool Entity::IsValid() const
{
    return (m_entityHandle != entt::null) && m_registry.valid(m_entityHandle);
}

Entity::operator bool() const
{
    return IsValid();
}

} // namespace Doodle