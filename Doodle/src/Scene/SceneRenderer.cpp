#include "SceneRenderer.h"
#include "ApplicationEvent.h"
#include "Component.h"
#include "Entity.h"
#include "EventManager.h"
#include "Scene.h"

namespace Doodle
{

void PrintBinary(const void *ptr, size_t size)
{
    uint8_t *bytePtr = reinterpret_cast<uint8_t *>(const_cast<void *>(ptr)); // 将 void * 转换为 uint8_t *
    std::ostringstream oss;
    for (size_t i = 0; i < size; i++)
    {
        for (int j = 7; j >= 0; j--)
        { // 从高位到低位输出每个字节
            oss << ((bytePtr[i] & (1 << j)) ? '1' : '0');
        }
        oss << ' '; // 每个字节之间加个空格
    }
    DOO_CORE_TRACE("Binary: {0}", oss.str());
}

SceneRenderer::SceneRenderer(Scene *scene) : m_scene(scene), m_registry(scene->m_registry)
{
    m_sceneUBO = UniformBuffer::Create(sizeof(UBOScene), true);
    m_pointLightsUBO = UniformBuffer::Create(sizeof(UBOPointLights), true);
    m_spotLightsUBO = UniformBuffer::Create(sizeof(UBOSpotLights), true);

    EventManager::Get()->AddListener<AppRenderEvent>(this, &SceneRenderer::Render);
}

SceneRenderer::~SceneRenderer()
{
    DOO_CORE_TRACE("SceneRenderer destroyed");
    EventManager::Get()->RemoveListener<AppRenderEvent>(this, &SceneRenderer::Render);
}

void SceneRenderer::Render()
{
    if (!m_scene->IsActive())
    {
        return;
    }

    auto cameraEntity = m_scene->GetMainCameraEntity();
    if (!cameraEntity)
    {
        return;
    }
    glm::mat4 view = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetModelMatrix());
    glm::mat4 projection = cameraEntity.GetComponent<CameraComponent>().GetProjectionMatrix();

    // Process lights
    {
        m_lightEnvironment = LightEnvironment();
        // Directional Lights
        {
            auto lights = m_registry.group<DirectionalLightComponent>(entt::get<TransformComponent>);
            uint32_t directionalLightIndex = 0;
            for (auto e : lights)
            {
                auto [transformComponent, lightComponent] =
                    lights.get<TransformComponent, DirectionalLightComponent>(e);
                glm::vec3 direction =
                    glm::normalize(glm::mat3(transformComponent.GetModelMatrix()) * glm::vec3(0.0f, 0.0f, -1.0f));
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
                auto pointLights = m_registry.group<PointLightComponent>(entt::get<TransformComponent>);
                m_lightEnvironment.PointLights.resize(pointLights.size());
                uint32_t pointLightIndex = 0;
                for (auto e : pointLights)
                {
                    Entity entity(m_scene, e);
                    auto [transformComponent, lightComponent] =
                        pointLights.get<TransformComponent, PointLightComponent>(e);
                    m_lightEnvironment.PointLights[pointLightIndex++] = {
                        transformComponent.Position, lightComponent.Radiance, lightComponent.Intensity,
                        lightComponent.MinRadius,    lightComponent.Radius,   lightComponent.Falloff,
                        lightComponent.SourceSize,
                    };
                }
            }
            // Spot Lights
            {
                auto spotLights = m_registry.group<SpotLightComponent>(entt::get<TransformComponent>);
                m_lightEnvironment.SpotLights.resize(spotLights.size());
                uint32_t spotLightIndex = 0;
                for (auto e : spotLights)
                {
                    Entity entity(m_scene, e);
                    auto [transformComponent, lightComponent] =
                        spotLights.get<TransformComponent, SpotLightComponent>(e);
                    glm::vec3 direction =
                        glm::normalize(glm::rotate(transformComponent.GetRotation(), glm::vec3(1.0f, 0.0f, 0.0f)));

                    m_lightEnvironment.SpotLights[spotLightIndex++] = {
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
    UBOScene sceneData = {};
    sceneData.DirectionalLight = m_lightEnvironment.DirectionalLights[0];
    // PrintBinary(&sceneData, sizeof(UBOScene));
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

    auto vaoView = m_registry.view<TransformComponent, VAOComponent, MaterialComponent>();
    for (auto entity : vaoView)
    {
        const auto &transform = vaoView.get<TransformComponent>(entity);
        const auto &vao = vaoView.get<VAOComponent>(entity);
        const auto &material = vaoView.get<MaterialComponent>(entity);

        glm::mat4 model = transform.GetModelMatrix();

        material.MaterialInstance->SetUniformMatrix4f("u_Model", model);
        material.MaterialInstance->SetUniformMatrix4f("u_View", view);
        material.MaterialInstance->SetUniformMatrix4f("u_Projection", projection);

        material.MaterialInstance->Bind();

        vao.Render();
    }

    auto meshView = m_registry.view<TransformComponent, MeshComponent, MaterialComponent>();
    for (auto entity : meshView)
    {

        const auto &transform = meshView.get<TransformComponent>(entity);
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

} // namespace Doodle
