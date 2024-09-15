#include "SceneRenderer.h"
#include "ApplicationEvent.h"
#include "Component.h"
#include "Entity.h"
#include "EventManager.h"
#include "FrameBuffer.h"
#include "PanelManager.h"
#include "RenderScope.h"
#include "SceneManager.h"
#include "ViewportPanel.h"

namespace Doodle
{

SceneRenderer::SceneRenderer()
{
    m_sceneUBO = UniformBuffer::Create(sizeof(UBOScene), true);
    m_pointLightsUBO = UniformBuffer::Create(sizeof(UBOPointLights), true);
    m_spotLightsUBO = UniformBuffer::Create(sizeof(UBOSpotLights), true);
    FramebufferAttachmentSpecification attachments = {FramebufferTextureFormat::RGBA8,
                                                      FramebufferTextureFormat::RED_INTEGER,
                                                      FramebufferTextureFormat::DEPTH24STENCIL8};
    FramebufferSpecification spec = {1920, 1080, attachments};
    m_frameBuffer = FrameBuffer::Create(spec);

    EventManager::Get()->AddListener<AppRenderEvent>(this, &SceneRenderer::Render);
}

SceneRenderer::~SceneRenderer()
{
    DOO_CORE_TRACE("SceneRenderer destroyed");
}

void SceneRenderer::Render()
{
    RenderScope<FrameBuffer> renderScope(m_frameBuffer.get());
    Renderer::Clear();
    auto scene = SceneManager::Get()->GetActiveScene();
    if (!scene || !scene->IsActive())
    {
        return;
    }

    auto cameraEntity = scene->GetMainCameraEntity();
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
            auto lights = scene->m_registry.group<DirectionalLightComponent>(entt::get<TransformComponent>);
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
                auto pointLights = scene->m_registry.group<PointLightComponent>(entt::get<TransformComponent>);
                m_lightEnvironment.PointLights.resize(pointLights.size());
                uint32_t pointLightIndex = 0;
                for (auto e : pointLights)
                {
                    Entity entity(scene.get(), e);
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
                auto spotLights = scene->m_registry.group<SpotLightComponent>(entt::get<TransformComponent>);
                m_lightEnvironment.SpotLights.resize(spotLights.size());
                uint32_t spotLightIndex = 0;
                for (auto e : spotLights)
                {
                    Entity entity(scene.get(), e);
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
    sceneData.AmbientRadiance = glm::vec3(0.03f);
    sceneData.CameraPosition = cameraEntity.GetComponent<TransformComponent>().Position;
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

    auto vaoView = scene->m_registry.view<TransformComponent, VAOComponent, MaterialComponent>();
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

    auto meshView = scene->m_registry.view<TransformComponent, MeshComponent, MaterialComponent>();
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

bool SceneRenderer::OnEvent(ViewportResizeEvent &e)
{
    m_frameBuffer->Resize(e.GetWidth(), e.GetHeight());
    return false;
}

} // namespace Doodle
