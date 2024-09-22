#pragma once

#include "imgui.h"
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
        m_gtaoShader = ShaderLibrary::Get()->GetShader("gtao");
        m_spatialFilterShader = ShaderLibrary::Get()->GetShader("bilateral");
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
        auto occlusionMap = RenderPipeline::Get()->GetFrameBuffer("OcclusionMap");
        occlusionMap->Resize(gBuffer->GetWidth(), gBuffer->GetHeight());

        auto *scene = m_scene;
        auto &sceneData = scene->GetData();

        m_gtaoShader->Bind();
        m_gtaoShader->SetUniformTexture("u_GDepth", gBuffer->GetDepthAttachmentTextureHandle());
        m_gtaoShader->SetUniformTexture("u_NoiseTexture", m_blueNoiseTexture->GetTextureHandle());
        m_gtaoShader->SetUniformMatrix4f("u_View", sceneData.CameraData.View);
        m_gtaoShader->SetUniformMatrix4f("u_Projection", sceneData.CameraData.Projection);
        m_gtaoShader->SetUniformMatrix4f("u_InverseProjection", glm::inverse(sceneData.CameraData.Projection));
        m_gtaoShader->SetUniform2f("u_Resolution", {occlusionMap->GetWidth(), occlusionMap->GetHeight()});
        m_gtaoShader->SetUniform2f("u_PixelSize", {1.0f / occlusionMap->GetWidth(), 1.0f / occlusionMap->GetHeight()});
        m_gtaoShader->SetUniform1i("u_FrameCounter", Application::Time::GetFrameCount());

        m_gtaoShader->SetUniform1i("u_Slices", m_slices);
        m_gtaoShader->SetUniform1i("u_HorizonSteps", m_horizonSteps);
        m_gtaoShader->SetUniform1f("u_Radius", m_radius);
        m_gtaoShader->SetUniform1f("u_FalloffStart", m_falloffStart);
        Renderer::RenderFullscreenQuad(gBuffer, m_gtaoShader);

        m_spatialFilterShader->Bind();
        m_spatialFilterShader->SetUniform1f("u_SpatialSigma", m_spatialSigma);
        m_spatialFilterShader->SetUniform1f("u_ColorSigma", m_colorSigma);
        m_spatialFilterShader->SetUniform1i("u_FilterRadius", m_filterRadius);
        Renderer::RenderFullscreenQuad(occlusionMap, m_spatialFilterShader);
    }

    void OnLayout() override
    {
        ImGui::DragInt("Slices", &m_slices, 1, 1, 100);
        ImGui::DragInt("Horizon Steps", &m_horizonSteps, 1, 1, 100);
        ImGui::DragFloat("Radius", &m_radius, 0.1f, 0.1f, 10.0f);
        ImGui::DragFloat("Falloff Start", &m_falloffStart, 0.01f, 0.01f, 1.0f);

        ImGui::DragFloat("Spatial Sigma", &m_spatialSigma, 0.1f, 0.1f, 10.0f);
        ImGui::DragFloat("Color Sigma", &m_colorSigma, 0.1f, 0.1f, 10.0f);
        ImGui::DragInt("Filter Radius", &m_filterRadius, 1, 1, 10);
    }

private:
    std::shared_ptr<Shader> m_gtaoShader;
    std::shared_ptr<Shader> m_spatialFilterShader;
    std::shared_ptr<Texture2D> m_blueNoiseTexture;

    int m_slices = 2;
    int m_horizonSteps = 3;
    float m_radius = 2.0f;
    float m_falloffStart = 0.01f;

    float m_spatialSigma = 2.0f;
    float m_colorSigma = 1.0f;
    int m_filterRadius = 5;
};

} // namespace Doodle