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

} // namespace Doodle