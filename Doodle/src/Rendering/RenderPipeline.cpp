#include "RenderPipeline.h"
#include "BloomPass.h"
#include "GeometryPass.h"
#include "OcclusionPass.h"
#include "PreDepthPass.h"
#include "SceneRenderer.h"
#include "ShadingPass.h"
#include "ShadowPass.h"
#include "SkyboxPass.h"
#include "Utils.h"
#include <unordered_map>

namespace Doodle
{

RenderPipeline::RenderPipeline()
{
    m_uniformBuffers["SceneData"] = UniformBuffer::Create(sizeof(UBOScene), true);
    m_uniformBuffers["PointLightData"] = UniformBuffer::Create(sizeof(UBOPointLights), true);
    m_uniformBuffers["SpotLightData"] = UniformBuffer::Create(sizeof(UBOSpotLights), true);
    m_uniformBuffers["AreaLightData"] = UniformBuffer::Create(sizeof(UBOAreaLights), true);

    m_frameBuffers["PreDepthMap"] = FrameBuffer::Create({1920, 1080, {FramebufferTextureFormat::Depth}});
    m_frameBuffers["GBuffer"] = FrameBuffer::Create(
        {1920,
         1080,
         {FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::Depth}});
    m_frameBuffers["ShadowMap"] = FrameBuffer::Create({8192, 8192, {FramebufferTextureFormat::Depth}});

    m_frameBuffers["OcclusionMap"] = FrameBuffer::Create({1920, 1080, {FramebufferTextureFormat::RGBA8}});
}

void RenderPipeline::RegisterRenderPasses()
{
    CreateRenderPass<SkyboxPass>({"SkyboxPass", m_targetFrameBuffer});
    CreateRenderPass<PreDepthPass>({"PreDepthPass", m_targetFrameBuffer});
    CreateRenderPass<GeometryPass>({"GeometryPass", m_frameBuffers["GBuffer"]});
    CreateRenderPass<OcclusionPass>({"OcclusionPass", m_frameBuffers["OcclusionMap"]});
    CreateRenderPass<ShadowPass>({"ShadowPass", m_frameBuffers["ShadowMap"]});
    CreateRenderPass<ShadingPass>({"ShadingPass", m_targetFrameBuffer});
    CreateRenderPass<BloomPass>({"BloomPass", m_targetFrameBuffer});
}

std::unordered_map<std::string, std::shared_ptr<RenderPass>> RenderPipeline::GetRenderPasses()
{
    return m_renderPasses;
}

std::unordered_map<std::string, std::shared_ptr<FrameBuffer>> RenderPipeline::GetFrameBuffers()
{
    return m_frameBuffers;
}

void RenderPipeline::AddRenderPass(const std::string &name, std::shared_ptr<RenderPass> renderPass)
{
    m_renderPasses[name] = renderPass;
}

template <typename T> void RenderPipeline::CreateRenderPass(const RenderPassSpecification &specification)
{
    m_renderPasses[specification.DebugName] = std::make_shared<T>(specification);
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
        s_UboScene.ShadowBias = sceneData.ShadowBias;
        s_UboScene.ShadowNormalBias = sceneData.ShadowNormalBias;
        s_UboScene.Resolution = {m_targetFrameBuffer->GetWidth(), m_targetFrameBuffer->GetHeight()};
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
        renderPass->GetSpecification().TargetFrameBuffer->Bind();
        renderPass->Execute();
        renderPass->GetSpecification().TargetFrameBuffer->Unbind();
    }
}

void RenderPipeline::SetTargetFrameBuffer(std::shared_ptr<FrameBuffer> targetFrameBuffer)
{
    m_targetFrameBuffer = targetFrameBuffer;
}

void RenderPipeline::SetUniformBuffer(const std::string &name, std::shared_ptr<UniformBuffer> uniformBuffer)
{
    m_uniformBuffers[name] = uniformBuffer;
}

void RenderPipeline::SetFrameBuffer(const std::string &name, std::shared_ptr<FrameBuffer> frameBuffer)
{
    m_frameBuffers[name] = frameBuffer;
}

void RenderPipeline::SetUniform1f(const std::string &name, float value)
{
    m_uniforms1f[name] = value;
}

void RenderPipeline::SetUniform2f(const std::string &name, glm::vec2 value)
{
    m_uniforms2f[name] = value;
}

void RenderPipeline::SetUniform3f(const std::string &name, glm::vec3 value)
{
    m_uniforms3f[name] = value;
}

void RenderPipeline::SetUniform4f(const std::string &name, glm::vec4 value)
{
    m_uniforms4f[name] = value;
}

void RenderPipeline::SetUniform1i(const std::string &name, int value)
{
    m_uniforms1i[name] = value;
}

void RenderPipeline::SetUniform2i(const std::string &name, glm::ivec2 value)
{
    m_uniforms2i[name] = value;
}

void RenderPipeline::SetUniform3i(const std::string &name, glm::ivec3 value)
{
    m_uniforms3i[name] = value;
}

void RenderPipeline::SetUniform4i(const std::string &name, glm::ivec4 value)
{
    m_uniforms4i[name] = value;
}

void RenderPipeline::SetUniformMatrix3f(const std::string &name, glm::mat3 value)
{
    m_uniforms3m[name] = value;
}

void RenderPipeline::SetUniformMatrix4f(const std::string &name, glm::mat4 value)
{
    m_uniforms4m[name] = value;
}

void RenderPipeline::SetUniformTexture(const std::string &name, std::shared_ptr<Texture> value)
{
    m_textures[name] = value;
}

std::shared_ptr<UniformBuffer> RenderPipeline::GetUniformBuffer(const std::string &name)
{
    return m_uniformBuffers[name];
}

std::shared_ptr<FrameBuffer> RenderPipeline::GetFrameBuffer(const std::string &name)
{
    return m_frameBuffers[name];
}

float RenderPipeline::GetUniform1f(const std::string &name)
{
    return m_uniforms1f[name];
}

glm::vec2 RenderPipeline::GetUniform2f(const std::string &name)
{
    return m_uniforms2f[name];
}

glm::vec3 RenderPipeline::GetUniform3f(const std::string &name)
{
    return m_uniforms3f[name];
}

glm::vec4 RenderPipeline::GetUniform4f(const std::string &name)
{
    return m_uniforms4f[name];
}

int RenderPipeline::GetUniform1i(const std::string &name)
{
    return m_uniforms1i[name];
}

glm::ivec2 RenderPipeline::GetUniform2i(const std::string &name)
{
    return m_uniforms2i[name];
}

glm::ivec3 RenderPipeline::GetUniform3i(const std::string &name)
{
    return m_uniforms3i[name];
}

glm::ivec4 RenderPipeline::GetUniform4i(const std::string &name)
{
    return m_uniforms4i[name];
}

glm::mat3 RenderPipeline::GetUniformMatrix3f(const std::string &name)
{
    return m_uniforms3m[name];
}

glm::mat4 RenderPipeline::GetUniformMatrix4f(const std::string &name)
{
    return m_uniforms4m[name];
}

std::shared_ptr<Texture> RenderPipeline::GetUniformTexture(const std::string &name)
{
    return m_textures[name];
}

} // namespace Doodle