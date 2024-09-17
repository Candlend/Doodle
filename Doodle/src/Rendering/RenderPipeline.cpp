#include "RenderPipeline.h"
#include "ShadingPass.h"
#include "SkyboxPass.h"
#include "Utils.h"

namespace Doodle
{

RenderPipeline::RenderPipeline()
{
    m_uniformBuffers["SceneData"] = UniformBuffer::Create(sizeof(UBOScene), true);
    m_uniformBuffers["PointLightData"] = UniformBuffer::Create(sizeof(UBOPointLights), true);
    m_uniformBuffers["SpotLightData"] = UniformBuffer::Create(sizeof(UBOSpotLights), true);
    m_uniformBuffers["AreaLightData"] = UniformBuffer::Create(sizeof(UBOAreaLights), true);
    RegisterRenderPasses();
}
void RenderPipeline::RegisterRenderPasses()
{
    CreateRenderPass<SkyboxPass>("SkyboxPass", {SceneRenderer::Get()->GetFrameBuffer()});
    CreateRenderPass<ShadingPass>("ShadingPass", {SceneRenderer::Get()->GetFrameBuffer()});
}
void RenderPipeline::AddRenderPass(const std::string &name, std::shared_ptr<RenderPass> renderPass)
{
    m_renderPasses[name] = renderPass;
}

template <typename T>
void RenderPipeline::CreateRenderPass(const std::string &name, const RenderPassSpecification &specification)
{
    m_renderPasses[name] = std::make_shared<T>(specification);
}
void RenderPipeline::RemoveRenderPass(const std::string &name)
{
    m_renderPasses.erase(name);
}
std::shared_ptr<RenderPass> RenderPipeline::GetRenderPass(const std::string &name)
{
    return m_renderPasses[name];
}
void RenderPipeline::BeginScene(Scene *scene)
{
    m_scene = scene;
    for (const auto &[name, renderPass] : m_renderPasses)
    {
        renderPass->m_scene = scene;
        renderPass->BeginScene();
    }
}
void RenderPipeline::EndScene()
{
    for (const auto &[name, renderPass] : m_renderPasses)
    {
        renderPass->EndScene();
    }
}
void RenderPipeline::Execute()
{
    auto &sceneData = m_scene->GetData();
    {
        static UBOScene s_UboScene = {};
        // 赋值directional light
        for (int i = 0; i < 4; i++)
        {
            s_UboScene.DirectionalLights[i] = sceneData.LightData.DirectionalLights[i];
        }
        s_UboScene.CameraPosition = sceneData.CameraData.Position;
        s_UboScene.EnvironmentIntensity = sceneData.EnvironmentData.Intensity;
        s_UboScene.EnvironmentRotation = sceneData.EnvironmentData.Rotation;
        m_uniformBuffers["SceneData"]->SetSubData(&s_UboScene, sizeof(UBOScene));

        static UBOPointLights s_UboPointLights = {};
        const std::vector<PointLight> &pointLightsVec = sceneData.LightData.PointLights;
        s_UboPointLights.Count = pointLightsVec.size();
        std::memcpy(s_UboPointLights.PointLights, pointLightsVec.data(), sceneData.LightData.GetPointLightsSize());
        m_uniformBuffers["PointLightData"]->SetSubData(&s_UboPointLights, sizeof(UBOPointLights));

        static UBOSpotLights s_UboSpotLights = {};
        const std::vector<SpotLight> &spotLightsVec = sceneData.LightData.SpotLights;
        s_UboSpotLights.Count = spotLightsVec.size();
        std::memcpy(s_UboSpotLights.SpotLights, spotLightsVec.data(), sceneData.LightData.GetSpotLightsSize());
        m_uniformBuffers["SpotLightData"]->SetSubData(&s_UboSpotLights, sizeof(UBOSpotLights));

        static UBOAreaLights s_UboAreaLights = {};
        const std::vector<AreaLight> &areaLightsVec = sceneData.LightData.AreaLights;
        s_UboAreaLights.Count = areaLightsVec.size();
        std::memcpy(s_UboAreaLights.AreaLights, areaLightsVec.data(), sceneData.LightData.GetAreaLightsSize());
        m_uniformBuffers["AreaLightData"]->SetSubData(&s_UboAreaLights, sizeof(UBOAreaLights));
    }

    for (const auto &[name, renderPass] : m_renderPasses)
    {
        renderPass->GetSpecification().TargetFramebuffer->Bind();
        renderPass->Execute();
        renderPass->GetSpecification().TargetFramebuffer->Unbind();
    }
}
} // namespace Doodle