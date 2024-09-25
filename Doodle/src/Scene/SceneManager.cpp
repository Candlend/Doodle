#include <rfl/json/write.hpp>

#include "AssetManager.h"
#include "Component.h"
#include "Entity.h"
#include "LightComponent.h"
#include "Scene.h"
#include "SceneAsset.h"
#include "SceneManager.h"
#include "UUID.h"

namespace Doodle
{

template <typename T>
bool TryLoadComponent(std::string componentName, rfl::Generic::Object &componentData, Entity &entity)
{
    if (componentName != T::GetStaticName())
    {
        return false;
    }
    if (entity.HasComponent<T>())
    {
        auto &component = entity.GetComponent<T>();
        component.DeserializeFromObject(componentData);
    }
    else
    {
        auto &component = entity.AddComponent<T>();
        component.DeserializeFromObject(componentData);
    }
    return true;
}

Entity SceneManager::DeserializeEntity(const EntityInfo &entityInfo)
{
    UUID uuid = entityInfo.UUID;
    std::string name = entityInfo.Name;
    auto entity = m_activeScene->CreateEntity(name, uuid);
    DOO_CORE_DEBUG("Entity {0} ({1}) deserialized", name, uuid.ToString());

    for (const auto &[componentName, componentData] : entityInfo.Components)
    {
        auto object = componentData.to_object().value();
        TryLoadComponent<TransformComponent>(componentName, object, entity) ||
            TryLoadComponent<CameraComponent>(componentName, object, entity) ||
            TryLoadComponent<MeshComponent>(componentName, object, entity) ||
            TryLoadComponent<MaterialComponent>(componentName, object, entity) ||
            TryLoadComponent<DirectionalLightComponent>(componentName, object, entity) ||
            TryLoadComponent<PointLightComponent>(componentName, object, entity) ||
            TryLoadComponent<SpotLightComponent>(componentName, object, entity) ||
            TryLoadComponent<AreaLightComponent>(componentName, object, entity);
    }
    for (const auto &child : entityInfo.Children)
    {
        Entity childEntity = DeserializeEntity(child);
        childEntity.SetParent(entity);
    }
    return entity;
}

std::shared_ptr<Scene> SceneManager::LoadScene(const SceneInfo &sceneInfo)
{
    if (m_activeScene)
    {
        m_activeScene->EndScene();
    }
    m_activeScene = std::make_shared<Scene>();
    m_activeSceneInfo = sceneInfo;

    auto entities = sceneInfo.Entities;

    for (const auto &entityInfo : entities)
    {
        Entity entity = DeserializeEntity(entityInfo);
    }
    m_activeScene->BeginScene();
    return m_activeScene;
}

EntityInfo SceneManager::SerializeEntity(const Entity &entity)
{
    EntityInfo entityInfo;
    entityInfo.UUID = entity.GetUUID();
    entityInfo.Name = entity.GetName();

    for (auto *component : m_activeScene->GetComponents(entity.GetUUID()))
    {
        entityInfo.Components[component->GetName()] = component->SerializeToObject();
    }
    for (const auto &child : entity.GetChildren())
    {
        EntityInfo childInfo = SerializeEntity(child);
        entityInfo.Children.push_back(childInfo);
    }
    DOO_CORE_DEBUG("Entity {0} ({1}) serialized", entityInfo.Name, entityInfo.UUID.ToString());
    return entityInfo;
}

SceneInfo SceneManager::GetSceneInfo()
{
    if (!m_activeScene)
    {
        DOO_CORE_WARN("No active scene found");
        return SceneInfo();
    }
    m_activeSceneInfo.Entities.clear();
    for (const auto &entity : m_activeScene->GetEntities())
    {
        EntityInfo entityInfo = SerializeEntity(entity);
        m_activeSceneInfo.Entities.push_back(entityInfo);
    }
    return m_activeSceneInfo;
}

void SceneManager::SaveScene(const std::filesystem::path &filepath)
{
    if (!m_activeScene)
    {
        DOO_CORE_WARN("No active scene found");
        return;
    }
    auto sceneInfo = GetSceneInfo();
    auto uuid = sceneInfo.UUID;
    auto sceneAsset = AssetManager::Get()->GetAsset<SceneAsset>(uuid);
    sceneAsset->SetData(sceneInfo);
    if (filepath.empty())
    {
        sceneAsset->Save();
    }
    else
    {
        sceneAsset->SaveAs(filepath);
    }
}

} // namespace Doodle