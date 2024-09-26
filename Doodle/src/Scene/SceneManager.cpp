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

template <typename... Components>
bool TryLoadComponents(const std::string &componentName, rfl::Generic::Object &componentData, Entity &entity)
{
    return (TryLoadComponent<Components>(componentName, componentData, entity) || ...);
}

std::shared_ptr<Scene> SceneManager::LoadScene(const SceneInfo &sceneInfo)
{
    std::function<Entity(const EntityInfo &)> deserializeEntity = [this,
                                                                   &deserializeEntity](const EntityInfo &entityInfo) {
        UUID uuid = entityInfo.UUID;
        std::string name = entityInfo.Name;
        auto entity = m_activeScene->CreateEntity(name, uuid);
        DOO_CORE_DEBUG("Entity {0} ({1}) deserialized", name, uuid.ToString());

        for (const auto &[componentName, componentData] : entityInfo.Components)
        {
            auto object = componentData.to_object().value();
            TryLoadComponents<TransformComponent, CameraComponent, MeshComponent, MaterialComponent,
                              DirectionalLightComponent, PointLightComponent, SpotLightComponent, AreaLightComponent>(
                componentName, object, entity);
        }
        for (const auto &child : entityInfo.Children)
        {
            Entity childEntity = deserializeEntity(child);
            childEntity.SetParent(entity);
        }
        return entity;
    };

    if (m_activeScene)
    {
        m_activeScene->EndScene();
    }
    m_activeScene = std::make_shared<Scene>();
    m_activeSceneInfo = sceneInfo;

    auto entities = sceneInfo.Entities;

    for (const auto &entityInfo : entities)
    {
        Entity entity = deserializeEntity(entityInfo);
    }
    m_activeScene->BeginScene();
    return m_activeScene;
}

SceneInfo SceneManager::GetSceneInfo()
{
    std::function<EntityInfo(const Entity &)> serializeEntity = [this, &serializeEntity](const Entity &entity) {
        EntityInfo entityInfo;
        entityInfo.UUID = entity.GetUUID();
        entityInfo.Name = entity.GetName();

        for (auto *component : m_activeScene->GetComponents(entity.GetUUID()))
        {
            entityInfo.Components[component->GetName()] = component->SerializeToObject();
        }
        for (const auto &child : entity.GetChildren())
        {
            EntityInfo childInfo = serializeEntity(child);
            entityInfo.Children.push_back(childInfo);
        }
        DOO_CORE_DEBUG("Entity {0} ({1}) serialized", entityInfo.Name, entityInfo.UUID.ToString());
        return entityInfo;
    };

    if (!m_activeScene)
    {
        DOO_CORE_WARN("No active scene found");
        return SceneInfo();
    }
    m_activeSceneInfo.Entities.clear();
    for (const auto &entity : m_activeScene->GetEntities())
    {
        EntityInfo entityInfo = serializeEntity(entity);
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
    filepath.empty() ? sceneAsset->Save() : sceneAsset->SaveAs(filepath);
}

} // namespace Doodle