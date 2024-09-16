#include "Scene.h"
#include "Component.h"
#include "EditorCamera.h"
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

void Scene::LoadEnvironment(const std::string &filepath)
{
    m_sceneData.Environment = Environment::Load(filepath);
}

void Scene::BeginScene()
{
    m_active = true;
    SceneManager::Get()->m_activeScene = shared_from_this();
    DOO_CORE_TRACE("Scene <{0}> Activated", m_name);
    EventManager::Get()->Dispatch<SceneActivateEvent>(this);
}

void Scene::EndScene()
{
    m_active = false;
    if (SceneManager::Get()->m_activeScene == shared_from_this())
        SceneManager::Get()->m_activeScene = nullptr;
    DOO_CORE_TRACE("Scene <{0}> Deactivated", m_name);
    EventManager::Get()->Dispatch<SceneDeactivateEvent>(this);
}

void Scene::SetupSceneData()
{
    if (SceneManager::Get()->GetState() == SceneState::Editor)
    {
        auto *editorCamera = EditorCamera::Get();
        m_sceneData.CameraData.Position = editorCamera->GetPosition();
        m_sceneData.CameraData.View = editorCamera->GetViewMatrix();
        m_sceneData.CameraData.Projection = editorCamera->GetProjectionMatrix();
    }
    else
    {
        auto cameraEntity = GetMainCameraEntity();
        if (!cameraEntity)
        {
            return;
        }
        m_sceneData.CameraData.Position = cameraEntity.GetComponent<TransformComponent>().Position;
        m_sceneData.CameraData.View = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetModelMatrix());
        m_sceneData.CameraData.Projection = cameraEntity.GetComponent<CameraComponent>().GetProjectionMatrix();
    }

    m_sceneData.CameraData.ViewProjection = m_sceneData.CameraData.Projection * m_sceneData.CameraData.View;

    // Process lights
    m_sceneData.LightEnvironment = LightEnvironment();
    // Directional Lights
    {
        auto lights = m_registry.group<DirectionalLightComponent>(entt::get<TransformComponent>);
        uint32_t directionalLightIndex = 0;
        for (auto e : lights)
        {
            auto [transformComponent, lightComponent] = lights.get<TransformComponent, DirectionalLightComponent>(e);
            glm::vec3 direction =
                glm::normalize(glm::mat3(transformComponent.GetModelMatrix()) * glm::vec3(0.0f, 0.0f, -1.0f));
            DOO_CORE_ASSERT(directionalLightIndex < LightEnvironment::MAX_DIRECTIONAL_LIGHTS,
                            "More than {} directional lights in scene!", LightEnvironment::MAX_DIRECTIONAL_LIGHTS);
            m_sceneData.LightEnvironment.DirectionalLights[directionalLightIndex++] = {
                direction,
                lightComponent.Radiance,
                lightComponent.Intensity,
            };
        }
        // Point Lights
        {
            auto pointLights = m_registry.group<PointLightComponent>(entt::get<TransformComponent>);
            m_sceneData.LightEnvironment.PointLights.resize(pointLights.size());
            uint32_t pointLightIndex = 0;
            for (auto e : pointLights)
            {
                Entity entity(this, e);
                auto [transformComponent, lightComponent] = pointLights.get<TransformComponent, PointLightComponent>(e);
                m_sceneData.LightEnvironment.PointLights[pointLightIndex++] = {
                    transformComponent.Position, lightComponent.Radiance, lightComponent.Intensity,
                    lightComponent.MinRadius,    lightComponent.Radius,   lightComponent.Falloff,
                    lightComponent.SourceSize,
                };
            }
        }
        // Spot Lights
        {
            auto spotLights = m_registry.group<SpotLightComponent>(entt::get<TransformComponent>);
            m_sceneData.LightEnvironment.SpotLights.resize(spotLights.size());
            uint32_t spotLightIndex = 0;
            for (auto e : spotLights)
            {
                Entity entity(this, e);
                auto [transformComponent, lightComponent] = spotLights.get<TransformComponent, SpotLightComponent>(e);
                glm::vec3 direction =
                    glm::normalize(glm::rotate(transformComponent.GetRotation(), glm::vec3(1.0f, 0.0f, 0.0f)));

                m_sceneData.LightEnvironment.SpotLights[spotLightIndex++] = {
                    transformComponent.Position,
                    direction,
                    lightComponent.Radiance,
                    lightComponent.Intensity,
                    lightComponent.AngleAttenuation,
                    lightComponent.Range,
                    lightComponent.Angle,
                    lightComponent.Falloff,
                };
            }
        }
    }
}

} // namespace Doodle