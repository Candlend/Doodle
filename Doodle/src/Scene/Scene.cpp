
#include "ApplicationEvent.h"
#include "Log.h"
#include "MaterialComponent.h"
#include "Renderable.h"
#include "SelectionManager.h"
#include "UUID.h"
#include "glm/fwd.hpp"
#include "pch.h"
#include <glad/glad.h>
#include <memory>
#include <vector>

#include "Component.h"
#include "EditorCamera.h"
#include "Entity.h"
#include "EventManager.h"
#include "Model.h"
#include "Scene.h"
#include "SceneEvent.h"
#include "SceneManager.h"

namespace Doodle
{

Scene::Scene(const std::string &name)
{
    m_name = name;
}

Scene::~Scene()
{
    m_registry.clear();
}

void Scene::OnUpdate()
{
    UpdateGlobalTransforms();
    UpdateSceneData();
}

void Scene::UpdateGlobalTransformTree(const TransformComponent &parentTransform, bool parentDirty)
{
    for (auto &entity : parentTransform.GetChildren())
    {
        auto &transform = entity.GetComponent<TransformComponent>();
        transform.UpdateGlobalTransform(parentTransform.GlobalTransform);
        bool dirty = parentDirty || transform.Dirty;
        UpdateGlobalTransformTree(transform, dirty);
    }
}

void Scene::UpdateGlobalTransforms()
{
    auto view = m_registry.view<TransformComponent>();
    for (auto entity : view)
    {
        auto &transform = view.get<TransformComponent>(entity);
        if (transform.ParentHandle == UUID::Nil())
        {
            auto &globalTransform = transform.GlobalTransform;
            transform.UpdateGlobalTransform();
            UpdateGlobalTransformTree(transform, transform.Dirty);
        }
    }
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

Entity Scene::CreateEntity(const std::string &name, const UUID &uuid)
{
    auto entity = Entity::Create(this);
    entity.AddComponent<UUIDComponent>(uuid);
    m_entityMap[entity.GetComponent<UUIDComponent>()] = entity;
    entity.AddComponent<NameComponent>(name);
    entity.AddComponent<TransformComponent>();
    m_entityComponents[entity.GetComponent<UUIDComponent>()] = {&entity.GetComponent<TransformComponent>()};

    DOO_CORE_DEBUG("Entity {0} ({1}) created", entity.GetName(), entity.GetUUID().ToString());
    return entity;
}

Entity Scene::ProcessModelNode(ModelNode node)
{
    Entity entity = CreateEntity(node.Name);
    for (auto &[meshName, mesh] : node.Meshes)
    {
        Entity meshEntity = CreateEntity(meshName);
        meshEntity.AddComponent<MeshComponent>(mesh);
        meshEntity.GetComponent<TransformComponent>().SetParent(entity);
        auto material = StandardMaterial::Create();
        for (auto &[name, texture] : mesh->GetTextures())
        {
            material->SetUniformTexture(name, texture);
        }
        for (auto &[name, value] : mesh->GetUniform1f())
        {
            material->SetUniform1f(name, value);
        }
        for (auto &[name, value] : mesh->GetUniform4f())
        {
            material->SetUniform4f(name, value);
        }
        meshEntity.AddComponent<MaterialComponent>(material);
    }

    for (auto &childNode : node.Children)
    {
        auto child = ProcessModelNode(childNode);
        child.GetComponent<TransformComponent>().SetParent(entity);
    }
    return entity;
}

Entity Scene::CreateEntityFromModel(std::shared_ptr<Model> model)
{
    return ProcessModelNode(model->GetRootNode());
}

Entity Scene::FindEntity(const std::string &name) const
{
    auto view = m_registry.view<NameComponent>();
    for (auto entity : view)
    {
        std::string tag = view.get<NameComponent>(entity);
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
    auto view = m_registry.view<UUIDComponent>();
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
    m_entityMap[entity.GetComponent<UUIDComponent>()] = entity;
}

void Scene::RemoveEntity(const UUID &id)
{
    if (SelectionManager::IsSelected(SelectionContext::Global, id))
    {
        SelectionManager::Deselect(SelectionContext::Global, id);
    }
    auto entity = m_entityMap[id];
    auto parent = entity.GetParent();
    if (parent)
    {
        parent.GetComponent<TransformComponent>().RemoveChild(entity);
    }
    for (auto &child : entity.GetChildren())
    {
        DestroyEntity(child);
    }
    m_entityMap.erase(id);
    m_entityComponents.erase(id);
    auto entityHandle = entity.GetEntityHandle();
    m_registry.destroy(entityHandle);
}

void Scene::DestroyEntity(const Entity &entity)
{
    UUID id = entity.GetComponent<UUIDComponent>();
    RemoveEntity(id);
}

void Scene::LoadEnvironment(const std::filesystem::path &filepath)
{
    const uint32_t CUBEMAP_SIZE = 2048;
    const uint32_t IRRADIANCE_MAP_SIZE = 32;

    TextureSpecification spec;
    spec.Width = CUBEMAP_SIZE;
    spec.Height = CUBEMAP_SIZE;
    spec.Format = TextureFormat::RGBA16F;
    spec.Wrap = TextureWrap::ClampToEdge;
    spec.Filter = TextureFilter::MipmapLinear;

    std::shared_ptr<TextureCube> envUnfiltered = TextureCube::Create(spec);
    static std::shared_ptr<Shader> s_EquirectangularConversionShader, s_EnvFilteringShader, s_EnvIrradianceShader;
    if (!s_EquirectangularConversionShader)
        s_EquirectangularConversionShader = Shader::Create("assets/shaders/equirectangularToCubeMap.glsl");

    TextureSpecification equirectParams;
    equirectParams.Format = TextureFormat::RGBA16F;
    std::shared_ptr<Texture2D> envEquirect = Texture2D::Create(filepath, equirectParams);
    DOO_CORE_ASSERT(envEquirect->GetFormat() == TextureFormat::RGBA16F, "Texture is not HDR!");

    s_EquirectangularConversionShader->Bind();
    envEquirect->Bind();
    Renderer::Submit([envUnfiltered, CUBEMAP_SIZE, envEquirect]() {
        glBindImageTexture(0, envUnfiltered->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
        glDispatchCompute(CUBEMAP_SIZE / 32, CUBEMAP_SIZE / 32, 6);
        glGenerateTextureMipmap(envUnfiltered->GetRendererID());
    });

    if (!s_EnvFilteringShader)
        s_EnvFilteringShader = Shader::Create("assets/shaders/environmentMipFilter.glsl");

    std::shared_ptr<TextureCube> envFiltered = TextureCube::Create(spec);

    Renderer::Submit([envUnfiltered, envFiltered]() {
        glCopyImageSubData(envUnfiltered->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
                           envFiltered->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0, envFiltered->GetWidth(),
                           envFiltered->GetHeight(), 6);
    });

    s_EnvFilteringShader->Bind();
    envUnfiltered->Bind();

    Renderer::Submit([envUnfiltered, envFiltered, CUBEMAP_SIZE]() {
        const float DELTA_ROUGHNESS = 1.0f / glm::max((envFiltered->GetMipLevelCount() - 1.0f), 1.0f);
        for (int level = 1, size = CUBEMAP_SIZE / 2; level < envFiltered->GetMipLevelCount();
             level++, size /= 2) // <= ?
        {
            const GLuint NUM_GROUPS = glm::max(1, size / 32);
            glBindImageTexture(0, envFiltered->GetRendererID(), level, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
            glProgramUniform1f(s_EnvFilteringShader->GetRendererID(), 0, level * DELTA_ROUGHNESS);
            glDispatchCompute(NUM_GROUPS, NUM_GROUPS, 6);
        }
    });

    if (!s_EnvIrradianceShader)
        s_EnvIrradianceShader = Shader::Create("assets/shaders/environmentIrradiance.glsl");

    spec.Width = IRRADIANCE_MAP_SIZE;
    spec.Height = IRRADIANCE_MAP_SIZE;
    spec.Filter = TextureFilter::Linear;

    std::shared_ptr<TextureCube> irradianceMap = TextureCube::Create(spec);
    s_EnvIrradianceShader->Bind();
    envFiltered->Bind();
    Renderer::Submit([irradianceMap]() {
        glBindImageTexture(0, irradianceMap->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
        glDispatchCompute(irradianceMap->GetWidth() / 32, irradianceMap->GetHeight() / 32, 6);
    });

    m_sceneData.EnvironmentData = {envFiltered, irradianceMap};
}

void Scene::BeginScene()
{
    m_active = true;
    SceneManager::Get()->m_activeScene = shared_from_this();
    DOO_CORE_TRACE("Scene <{0}> Activated", m_name);
    EventManager::Get()->AddListener<AppUpdateEvent>(this, &Scene::OnUpdate, ExecutionOrder::Last);
    OnUpdate();
    EventManager::Get()->Dispatch<SceneActivateEvent>(this);
}

void Scene::EndScene()
{
    m_active = false;
    if (SceneManager::Get()->m_activeScene == shared_from_this())
        SceneManager::Get()->m_activeScene = nullptr;
    DOO_CORE_TRACE("Scene <{0}> Deactivated", m_name);
    EventManager::Get()->Dispatch<SceneDeactivateEvent>(this);
    EventManager::Get()->RemoveListener<AppUpdateEvent>(this, &Scene::OnUpdate);
}

void Scene::UpdateSceneData()
{
    if (SceneManager::Get()->GetState() == SceneState::Editor)
    {
        auto editorCamera = EditorCamera::Get();
        m_sceneData.CameraData.Position = editorCamera->GetPosition();
        m_sceneData.CameraData.View = editorCamera->GetViewMatrix();
        m_sceneData.CameraData.Projection = editorCamera->GetProjectionMatrix();
        m_sceneData.CameraData.Near = editorCamera->GetNearClip();
        m_sceneData.CameraData.Far = editorCamera->GetFarClip();
    }
    else
    {
        auto cameraEntity = GetMainCameraEntity();
        if (!cameraEntity)
        {
            return;
        }
        m_sceneData.CameraData.Position = cameraEntity.GetComponent<TransformComponent>().GetPosition();
        m_sceneData.CameraData.View =
            glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransformMatrix());
        m_sceneData.CameraData.Projection = cameraEntity.GetComponent<CameraComponent>().GetProjectionMatrix();
        m_sceneData.CameraData.Near = cameraEntity.GetComponent<CameraComponent>().Camera->GetNearClip();
        m_sceneData.CameraData.Far = cameraEntity.GetComponent<CameraComponent>().Camera->GetFarClip();
    }

    m_sceneData.CameraData.ViewProjection = m_sceneData.CameraData.Projection * m_sceneData.CameraData.View;

    // Process lights
    m_sceneData.LightData = LightData();
    // Directional Lights
    {
        auto lights = m_registry.group<DirectionalLightComponent>(entt::get<TransformComponent>);
        uint32_t directionalLightIndex = 0;
        for (auto e : lights)
        {
            auto [transformComponent, lightComponent] = lights.get<TransformComponent, DirectionalLightComponent>(e);
            glm::vec3 direction =
                glm::normalize(glm::mat3(transformComponent.GetTransformMatrix()) * glm::vec3(0.0f, 0.0f, -1.0f));
            DOO_CORE_ASSERT(directionalLightIndex < LightData::MAX_DIRECTIONAL_LIGHTS,
                            "More than {} directional lights in scene!", LightData::MAX_DIRECTIONAL_LIGHTS);
            m_sceneData.LightData.DirectionalLights[directionalLightIndex++] = {
                direction,
                lightComponent.Radiance,
                lightComponent.Intensity,
            };
        }
        // Point Lights
        {
            auto pointLights = m_registry.group<PointLightComponent>(entt::get<TransformComponent>);
            m_sceneData.LightData.PointLights.resize(pointLights.size());
            uint32_t pointLightIndex = 0;
            for (auto e : pointLights)
            {
                Entity entity(this, e);
                auto [transformComponent, lightComponent] = pointLights.get<TransformComponent, PointLightComponent>(e);
                m_sceneData.LightData.PointLights[pointLightIndex++] = {
                    transformComponent.GetPosition(), lightComponent.Radiance, lightComponent.Intensity,
                    lightComponent.MinRange, lightComponent.Range};
            }
        }
        // Spot Lights
        {
            auto spotLights = m_registry.group<SpotLightComponent>(entt::get<TransformComponent>);
            m_sceneData.LightData.SpotLights.resize(spotLights.size());
            uint32_t spotLightIndex = 0;
            for (auto e : spotLights)
            {
                Entity entity(this, e);
                auto [transformComponent, lightComponent] = spotLights.get<TransformComponent, SpotLightComponent>(e);
                glm::vec3 direction =
                    glm::normalize(glm::rotate(transformComponent.GetQuaternion(), glm::vec3(0.0f, 0.0f, -1.0f)));

                m_sceneData.LightData.SpotLights[spotLightIndex++] = {
                    transformComponent.GetPosition(), direction,
                    lightComponent.Radiance,          lightComponent.Intensity,
                    lightComponent.MinRange,          lightComponent.Range,
                    lightComponent.MinAngle,          lightComponent.Angle,
                };
            }
        }
        // Area Lights
        {
            auto areaLights = m_registry.group<AreaLightComponent>(entt::get<TransformComponent>);
            m_sceneData.LightData.AreaLights.resize(areaLights.size());
            uint32_t areaLightIndex = 0;
            for (auto e : areaLights)
            {
                Entity entity(this, e);
                auto [transformComponent, lightComponent] = areaLights.get<TransformComponent, AreaLightComponent>(e);
                glm::mat4 model = transformComponent.GetTransformMatrix();
                glm::vec2 size = lightComponent.Size;
                glm::vec3 points[4] = {
                    glm::vec3(-size.x / 2, -size.y / 2, 0.0f),
                    glm::vec3(-size.x / 2, size.y / 2, 0.0f),
                    glm::vec3(size.x / 2, size.y / 2, 0.0f),
                    glm::vec3(size.x / 2, -size.y / 2, 0.0f),
                };
                for (auto &point : points)
                {
                    point = glm::vec3(model * glm::vec4(point, 1.0f));
                }
                m_sceneData.LightData.AreaLights[areaLightIndex++] = {
                    points,
                    lightComponent.Radiance,
                    lightComponent.Intensity,
                    lightComponent.TwoSided,
                };
            }
        }
    }
}

} // namespace Doodle