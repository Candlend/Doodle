#include "Scene.h"
#include "Component.h"
#include "Entity.h"
#include "Log.h"
#include "SceneManager.h"

namespace Doodle
{

std::shared_ptr<Scene> Scene::Create()
{
    return std::make_shared<Scene>();
}

Scene::Scene()
{
}

Scene::~Scene()
{
}

std::shared_ptr<Entity> Scene::GetMainCameraEntity()
{
    auto cameraView = m_registry.view<CameraComponent>();
    for (auto entity : cameraView)
    {
        const auto &camera = cameraView.get<CameraComponent>(entity);
        if (camera.Primary)
        {
            return std::make_shared<Entity>(m_registry, entity);
        }
    }
    DOO_CORE_WARN("No primary camera found");
    return nullptr;
}

std::shared_ptr<Entity> Scene::CreateEntity(const std::string &name)
{
    auto entity = Entity::Create(m_registry);
    entity->AddComponent<IDComponent>();
    entity->AddComponent<TagComponent>(name);
    entity->AddComponent<TransformComponent>();
    m_entityMap[entity->GetComponent<IDComponent>()] = entity->GetEntityHandle();
    return entity;
}

void Scene::AddEntity(const std::shared_ptr<Entity> &entity)
{
    m_entityMap[entity->GetComponent<IDComponent>()] = entity->GetEntityHandle();
}

void Scene::RemoveEntity(const UUID &id)
{
    m_registry.destroy(m_entityMap[id]);
    m_entityMap.erase(id);
}

void Scene::DestroyEntity(const std::shared_ptr<Entity> &entity)
{
    m_registry.destroy(entity->GetEntityHandle());
    UUID id = entity->GetComponent<IDComponent>();
    m_entityMap.erase(id);
}

void Scene::BeginScene()
{
    m_active = true;
    SceneManager::Get()->m_activeScene = shared_from_this();
    DOO_CORE_TRACE("Scene <{0}> Started", m_name);
}

void Scene::EndScene()
{
    m_active = false;
    if (SceneManager::Get()->m_activeScene == shared_from_this())
        SceneManager::Get()->m_activeScene = nullptr;
    DOO_CORE_TRACE("Scene <{0}> Ended", m_name);
}

} // namespace Doodle