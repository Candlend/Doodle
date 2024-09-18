#pragma once

#include "Framebuffer.h"
#include "Material.h"
#include "RenderPipeline.h"
#include "Renderer.h"
#include "pch.h"

#include "Component.h"
#include "RenderPass.h"
#include <memory>

namespace Doodle
{

class DOO_API GeometryPass : public RenderPass
{
public:
    GeometryPass(const RenderPassSpecification &specification) : RenderPass(specification)
    {
        m_shader = ShaderLibrary::Get()->GetShader("gbuffer");
    }

    void BeginScene() override
    {
    }

    void EndScene() override
    {
    }

    void Execute() override
    {
        Renderer::SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        Renderer::Clear();
        auto preDepthMap = RenderPipeline::Get()->GetFrameBuffer("PreDepthMap");
        auto gBuffer = RenderPipeline::Get()->GetFrameBuffer("GBuffer");
        gBuffer->Resize(preDepthMap->GetWidth(), preDepthMap->GetHeight());
        preDepthMap->BlitTo(gBuffer, BufferFlags::Depth);

        auto *scene = m_scene;
        auto &sceneData = scene->GetData();

        m_shader->SetUniformMatrix4f("u_View", sceneData.CameraData.View);
        m_shader->SetUniformMatrix4f("u_Projection", sceneData.CameraData.Projection);
        m_shader->SetUniform1f("u_NearPlane", sceneData.CameraData.Near);
        m_shader->SetUniform1f("u_FarPlane", sceneData.CameraData.Far);

        Renderer::SetDepthTest(DepthTestType::LessEqual);
        auto vaoView = scene->View<TransformComponent, VAOComponent, MaterialComponent>();
        for (auto entity : vaoView)
        {
            const auto &transform = vaoView.get<TransformComponent>(entity);
            const auto &vao = vaoView.get<VAOComponent>(entity);
            const auto &material = vaoView.get<MaterialComponent>(entity);

            glm::mat4 model = transform.GetModelMatrix();
            auto normalScale = material.MaterialInstance->GetUniform1f("u_NormalScale");
            auto normalTexture = material.MaterialInstance->GetUniformTexture("u_NormalTexture");

            m_shader->SetUniformMatrix4f("u_Model", model);
            m_shader->SetUniform1f("u_NormalScale", normalScale);
            m_shader->SetUniformTexture("u_NormalTexture", normalTexture);
            m_shader->Bind();
            vao.Render();
        }

        auto meshView = scene->View<TransformComponent, MeshComponent, MaterialComponent>();
        for (auto entity : meshView)
        {
            const auto &transform = meshView.get<TransformComponent>(entity);
            const auto &mesh = meshView.get<MeshComponent>(entity);
            const auto &material = meshView.get<MaterialComponent>(entity);

            glm::mat4 model = transform.GetModelMatrix();
            auto normalScale = material.MaterialInstance->GetUniform1f("u_NormalScale");
            auto normalTexture = material.MaterialInstance->GetUniformTexture("u_NormalTexture");

            m_shader->SetUniformMatrix4f("u_Model", model);
            m_shader->SetUniform1f("u_NormalScale", normalScale);
            m_shader->SetUniformTexture("u_NormalTexture", normalTexture);
            m_shader->Bind();
            mesh.Render();
        }
        Renderer::SetDepthTest(DepthTestType::Less);
    }

private:
    std::shared_ptr<Shader> m_shader;
};

} // namespace Doodle