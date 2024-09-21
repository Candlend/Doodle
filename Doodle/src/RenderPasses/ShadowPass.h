#pragma once

#include "Material.h"
#include "RenderPipeline.h"
#include "pch.h"

#include "Component.h"
#include "RenderPass.h"
#include <memory>

namespace Doodle
{

class DOO_API ShadowPass : public RenderPass
{
public:
    ShadowPass(const RenderPassSpecification &specification) : RenderPass(specification)
    {
        m_shader = ShaderLibrary::Get()->GetShader("shadow");
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

        auto *scene = m_scene;
        auto &sceneData = scene->GetData();
        auto directionalLight = sceneData.LightData.DirectionalLights[0];
        if (directionalLight.Intensity == 0.0f)
        {
            return;
        }

        auto lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 20.0f);
        auto lightView = glm::lookAt(-directionalLight.Direction * 10.0f, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        m_shader->SetUniformMatrix4f("u_View", lightView);
        m_shader->SetUniformMatrix4f("u_Projection", lightProjection);

        RenderPipeline::Get()->SetUniformMatrix4f("u_LightSpaceMatrix", lightProjection * lightView);

        auto vaoView = scene->View<TransformComponent, VAOComponent, MaterialComponent>();
        for (auto entity : vaoView)
        {
            const auto &transform = vaoView.get<TransformComponent>(entity);
            const auto &vao = vaoView.get<VAOComponent>(entity);

            glm::mat4 model = transform.GetTransformMatrix();
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

            glm::mat4 model = transform.GetTransformMatrix();
            m_shader->SetUniformMatrix4f("u_Model", model);
            m_shader->Bind();
            mesh.Render();
            m_shader->Unbind();
        }
    }

    void OnLayout() override
    {
        auto &sceneData = m_scene->GetData();
        ImGui::DragFloat("Bias", &sceneData.ShadowBias, 0.001f, 0.0f, 1.0f);
        ImGui::DragFloat("Normal Bias", &sceneData.ShadowNormalBias, 0.001f, 0.0f, 1.0f);
    }

private:
    std::shared_ptr<Shader> m_shader;
};

} // namespace Doodle