#include "Scene.h"
#include "Component.h"
#include "Entity.h"
#include "EventManager.h"
#include "Log.h"
#include "SceneEvent.h"
#include "SceneManager.h"

namespace Doodle
{

std::shared_ptr<Scene> Scene::Create(const std::string &name)
{
    return std::make_shared<Scene>(name);
}

Scene::Scene(const std::string &name)
{
    m_name = name;
}

Scene::~Scene()
{
    DOO_CORE_TRACE("Scene <{0}> destroyed", m_name);
    m_registry.clear();
}

Entity Scene::GetMainCameraEntity()
{
    auto cameraView = m_registry.view<CameraComponent>();
    for (auto entity : cameraView)
    {
        const auto &camera = cameraView.get<CameraComponent>(entity);
        if (camera.Primary)
        {
            return Entity(this, entity);
        }
    }
    DOO_CORE_WARN("No primary camera found");
    return {};
}

Entity Scene::CreateEntity(const std::string &name)
{
    auto entity = Entity::Create(this);
    entity.AddComponent<IDComponent>();
    m_entityMap[entity.GetComponent<IDComponent>()] = entity;
    m_entityComponents[entity.GetComponent<IDComponent>()] = {&entity.GetComponent<IDComponent>()};
    entity.AddComponent<TagComponent>(name);
    entity.AddComponent<TransformComponent>();
    return entity;
}

Entity Scene::FindEntity(const std::string &name) const
{
    auto view = m_registry.view<TagComponent>();
    for (auto entity : view)
    {
        std::string tag = view.get<TagComponent>(entity);
        if (tag == name)
        {
            return Entity(this, entity);
        }
    }
    return {};
}

Entity Scene::GetEntity(const UUID &id) const
{
    return m_entityMap.at(id);
}

std::vector<Entity> Scene::GetEntities() const
{
    std::vector<Entity> entities;
    auto view = m_registry.view<IDComponent>();
    for (auto entity : view)
    {
        entities.push_back(Entity(this, entity));
    }
    return entities;
}

std::vector<BaseComponent *> Scene::GetComponents(const UUID &id) const
{
    return m_entityComponents.at(id);
}

void Scene::AddEntity(const Entity &entity)
{
    m_entityMap[entity.GetComponent<IDComponent>()] = entity;
}

void Scene::RemoveEntity(const UUID &id)
{
    m_registry.destroy(m_entityMap[id].GetEntityHandle());
    m_entityMap.erase(id);
    m_entityComponents.erase(id);
}

void Scene::DestroyEntity(const Entity &entity)
{
    m_registry.destroy(entity.GetEntityHandle());
    UUID id = entity.GetComponent<IDComponent>();
    m_entityMap.erase(id);
    m_entityComponents.erase(id);
}

void Scene::BeginScene()
{
    m_active = true;
    SceneManager::Get()->m_activeScene = shared_from_this();
    DOO_CORE_TRACE("Scene <{0}> Activated", m_name);
    EventManager::Get()->Dispatch<SceneActivateEvent>(*this);
}

void Scene::EndScene()
{
    m_active = false;
    if (SceneManager::Get()->m_activeScene == shared_from_this())
        SceneManager::Get()->m_activeScene = nullptr;
    DOO_CORE_TRACE("Scene <{0}> Deactivated", m_name);
    EventManager::Get()->Dispatch<SceneDeactivateEvent>(*this);
}

} // namespace Doodle