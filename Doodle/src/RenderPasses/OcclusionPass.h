#pragma once

#include "pch.h"

#include "Application.h"
#include "Component.h"
#include "Framebuffer.h"
#include "Material.h"
#include "RenderPass.h"
#include "RenderPipeline.h"
#include <memory>

namespace Doodle
{

class DOO_API OcclusionPass : public RenderPass
{
public:
    OcclusionPass(const RenderPassSpecification &specification) : RenderPass(specification)
    {
        m_shader = ShaderLibrary::Get()->GetShader("gtao");
        m_blueNoiseTexture = Texture2D::Create("assets/textures/blueNoise.png");
    }

    void BeginScene() override
    {
    }

    void EndScene() override
    {
    }

    void Execute() override
    {
        auto gBuffer = RenderPipeline::Get()->GetFrameBuffer("GBuffer");
        auto occlusionMap = RenderPipeline::Get()->GetFrameBuffer("OcculusionMap");
        occlusionMap->Resize(gBuffer->GetWidth(), gBuffer->GetHeight());

        auto *scene = m_scene;
        auto &sceneData = scene->GetData();

        m_shader->Bind();
        m_shader->SetUniformTexture("u_GDepth", gBuffer->GetDepthAttachmentTextureHandle());
        m_shader->SetUniformTexture("u_NoiseTexture", m_blueNoiseTexture->GetTextureHandle());
        m_shader->SetUniformMatrix4f("u_View", sceneData.CameraData.View);
        m_shader->SetUniformMatrix4f("u_Projection", sceneData.CameraData.Projection);
        m_shader->SetUniformMatrix4f("u_InverseProjection", glm::inverse(sceneData.CameraData.Projection));
        m_shader->SetUniform2f("u_Resolution", {gBuffer->GetWidth(), gBuffer->GetHeight()});
        m_shader->SetUniform2f("u_PixelSize", {1.0f / gBuffer->GetWidth(), 1.0f / gBuffer->GetHeight()});
        m_shader->SetUniform1i("u_FrameCounter", Application::Time::GetFrameCount());
        Renderer::RenderFullscreenQuad(gBuffer, m_shader);
    }

private:
    std::shared_ptr<Shader> m_shader;
    std::shared_ptr<Texture2D> m_blueNoiseTexture;
};

} // namespace Doodle