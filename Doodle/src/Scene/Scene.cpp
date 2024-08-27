#include "Scene.h"
#include "Entity.h"
#include "SceneManager.h"

namespace Doodle
{

std::shared_ptr<Scene> Scene::Create()
{
    return std::make_shared<Scene>();
}

Scene::Scene()
{
    m_sceneUBO = UniformBuffer::Create(sizeof(UBOScene), true);
    m_pointLightsUBO = UniformBuffer::Create(sizeof(UBOPointLights), true);
    m_spotLightsUBO = UniformBuffer::Create(sizeof(UBOSpotLights), true);
}

std::shared_ptr<Entity> Scene::CreateEntity(const std::string &name)
{
    if (m_entities.find(name) != m_entities.end())
    {
        DOO_CORE_WARN("Entity with name {0} already exists", name);
    }
    auto entity = Entity::Create(m_registry);
    m_entities[name] = entity;
    return entity;
}

std::shared_ptr<Entity> Scene::GetEntity(const std::string &name) const
{
    return m_entities.at(name);
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

void Scene::Render()
{
    auto cameraEntity = GetMainCameraEntity();
    if (!cameraEntity)
    {
        return;
    }
    glm::mat4 view = glm::inverse(cameraEntity->GetComponent<Transform>().GetModelMatrix());
    glm::mat4 projection = cameraEntity->GetComponent<CameraComponent>().GetProjectionMatrix();

    // Process lights
    {
        m_lightEnvironment = LightEnvironment();
        // Directional Lights
        {
            auto lights = m_registry.group<DirectionalLightComponent>(entt::get<Transform>);
            uint32_t directionalLightIndex = 0;
            for (auto entity : lights)
            {
                auto [transformComponent, lightComponent] = lights.get<Transform, DirectionalLightComponent>(entity);
                glm::vec3 direction = -glm::normalize(glm::mat3(transformComponent.GetModelMatrix()) * glm::vec3(1.0f));
                DOO_CORE_ASSERT(directionalLightIndex < LightEnvironment::MAX_DIRECTIONAL_LIGHTS,
                                "More than {} directional lights in scene!", LightEnvironment::MAX_DIRECTIONAL_LIGHTS);
                m_lightEnvironment.DirectionalLights[directionalLightIndex++] = {
                    direction,
                    lightComponent.Radiance,
                    lightComponent.Intensity,
                };
            }
            // Point Lights
            {
                auto pointLights = m_registry.group<PointLightComponent>(entt::get<Transform>);
                m_lightEnvironment.PointLights.resize(pointLights.size());
                uint32_t pointLightIndex = 0;
                for (auto e : pointLights)
                {
                    Entity entity(m_registry, e);
                    auto [transformComponent, lightComponent] = pointLights.get<Transform, PointLightComponent>(e);
                    auto transform = entity.GetComponent<Transform>();
                    m_lightEnvironment.PointLights[pointLightIndex++] = {
                        transform.Position,        lightComponent.Radiance, lightComponent.Intensity,
                        lightComponent.MinRadius,  lightComponent.Radius,   lightComponent.Falloff,
                        lightComponent.SourceSize,
                    };
                }
            }
            // Spot Lights
            {
                auto spotLights = m_registry.group<SpotLightComponent>(entt::get<Transform>);
                m_lightEnvironment.SpotLights.resize(spotLights.size());
                uint32_t spotLightIndex = 0;
                for (auto e : spotLights)
                {
                    Entity entity(m_registry, e);
                    auto [transformComponent, lightComponent] = spotLights.get<Transform, SpotLightComponent>(e);
                    auto transform = entity.GetComponent<Transform>();
                    glm::vec3 direction =
                        glm::normalize(glm::rotate(transform.GetRotation(), glm::vec3(1.0f, 0.0f, 0.0f)));

                    m_lightEnvironment.SpotLights[spotLightIndex++] = {
                        transform.Position,
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
    UBOScene sceneData = {};
    sceneData.DirectionalLight = m_lightEnvironment.DirectionalLights[0];
    m_sceneUBO->SetSubData(&sceneData, sizeof(UBOScene));

    UBOPointLights pointLightData = {};
    const std::vector<PointLight> &pointLightsVec = m_lightEnvironment.PointLights;
    pointLightData.Count = pointLightsVec.size();
    std::memcpy(pointLightData.PointLights, pointLightsVec.data(), m_lightEnvironment.GetPointLightsSize());
    m_pointLightsUBO->SetSubData(&pointLightData, sizeof(UBOPointLights));

    UBOSpotLights spotLightData = {};
    const std::vector<SpotLight> &spotLightsVec = m_lightEnvironment.SpotLights;
    spotLightData.Count = spotLightsVec.size();
    std::memcpy(spotLightData.SpotLights, spotLightsVec.data(), m_lightEnvironment.GetSpotLightsSize());
    m_spotLightsUBO->SetSubData(&spotLightData, sizeof(UBOSpotLights));

    m_sceneUBO->Bind(0);
    m_pointLightsUBO->Bind(1);
    m_spotLightsUBO->Bind(2);

    auto vaoView = m_registry.view<Transform, VAOComponent, MaterialComponent>();
    for (auto entity : vaoView)
    {
        const auto &transform = vaoView.get<Transform>(entity);
        const auto &vao = vaoView.get<VAOComponent>(entity);
        const auto &material = vaoView.get<MaterialComponent>(entity);

        glm::mat4 model = transform.GetModelMatrix();

        material.MaterialInstance->SetUniformMatrix4f("u_Model", model);
        material.MaterialInstance->SetUniformMatrix4f("u_View", view);
        material.MaterialInstance->SetUniformMatrix4f("u_Projection", projection);

        material.MaterialInstance->Bind();
        vao.Render();
    }

    auto meshView = m_registry.view<Transform, MeshComponent, MaterialComponent>();
    for (auto entity : meshView)
    {

        const auto &transform = meshView.get<Transform>(entity);
        const auto &mesh = meshView.get<MeshComponent>(entity);
        const auto &material = meshView.get<MaterialComponent>(entity);

        glm::mat4 model = transform.GetModelMatrix();

        material.MaterialInstance->SetUniformMatrix4f("u_Model", model);
        material.MaterialInstance->SetUniformMatrix4f("u_View", view);
        material.MaterialInstance->SetUniformMatrix4f("u_Projection", projection);

        material.MaterialInstance->Bind();
        mesh.Render();
    }
}

void Scene::Begin()
{
    SceneManager::Get().m_activeScene = shared_from_this();
}

void Scene::End()
{
    SceneManager::Get().m_activeScene = nullptr;
}

} // namespace Doodle