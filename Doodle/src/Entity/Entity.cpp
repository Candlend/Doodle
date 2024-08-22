#include "Entity.h"

namespace Doodle
{

Entity::Entity(entt::registry &registry) : m_registry(registry), m_id(registry.create())
{
}

entt::entity Entity::GetId() const
{
    return m_id;
}

} // namespace Doodle