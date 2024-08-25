#include "Entity.h"

#include "Component.h"
#include "Scene.h"

namespace Doodle
{

std::shared_ptr<Entity> Entity::Create(entt::registry &registry)
{
    return std::make_shared<Entity>(registry);
}

Entity::Entity(entt::registry &registry, entt::entity id) : m_registry(registry), m_id(id)
{
}

Entity::Entity(entt::registry &registry) : m_registry(registry)
{
    m_id = m_registry.create();
    AddComponent<Transform>();
}

bool Entity::IsValid() const
{
    return (m_id != entt::null) && m_registry.valid(m_id);
}

Entity::operator bool() const
{
    return IsValid();
}

} // namespace Doodle