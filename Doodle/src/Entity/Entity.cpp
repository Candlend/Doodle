#include "Entity.h"
#include <algorithm>

namespace Doodle
{

std::shared_ptr<Entity> Entity::Create(entt::registry &registry)
{
    return std::make_shared<Entity>(registry);
}

Entity::Entity(entt::registry &registry) : m_registry(registry), m_id(registry.create())
{
    AddComponent<Transform>();
}

entt::entity Entity::GetId() const
{
    return m_id;
}

} // namespace Doodle