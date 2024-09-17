#pragma once

#include "Framebuffer.h"
#include "Material.h"
#include "RenderPipeline.h"
#include "pch.h"

#include "Component.h"
#include "RenderPass.h"
#include <memory>

namespace Doodle
{

class DOO_API PreDepthPass : public RenderPass
{
public:
    PreDepthPass(const RenderPassSpecification &specification) : RenderPass(specification)
    {
        m_shader = ShaderLibrary::Get()->GetShader("preDepth");
    }

    void BeginScene() override
    {
    }

    void EndScene() override
    {
    }

    void Execute() override
    {
        auto *scene = m_scene;
        auto &sceneData = scene->GetData();

        m_shader->SetUniformMatrix4f("u_View", sceneData.CameraData.View);
        m_shader->SetUniformMatrix4f("u_Projection", sceneData.CameraData.Projection);

        auto vaoView = scene->View<TransformComponent, VAOComponent, MaterialComponent>();
        for (auto entity : vaoView)
        {
            const auto &transform = vaoView.get<TransformComponent>(entity);
            const auto &vao = vaoView.get<VAOComponent>(entity);

            glm::mat4 model = transform.GetModelMatrix();
            m_shader->SetUniformMatrix4f("u_Model", glm::mat4(0.123f) + model);
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
            m_shader->SetUniformMatrix4f("u_Model", model);
            m_shader->Bind();
            mesh.Render();
            m_shader->Unbind();
        }
        auto preDepthMap = RenderPipeline::Get()->GetFrameBuffer("PreDepthMap");
        auto targetFrameBuffer = GetSpecification().TargetFrameBuffer;
        preDepthMap->Resize(targetFrameBuffer->GetSpecification().Width, targetFrameBuffer->GetSpecification().Height);
        targetFrameBuffer->BlitTo(preDepthMap);
    }

private:
    std::shared_ptr<Shader> m_shader;
};

} // namespace Doodle