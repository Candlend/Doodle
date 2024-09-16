#pragma once

#include "RenderPipeline.h"
#include "pch.h"

#include "Component.h"
#include "RenderPass.h"

namespace Doodle
{

class ShadingPass : public RenderPass
{
public:
    ShadingPass(const RenderPassSpecification &specification) : RenderPass(specification)
    {
    }

    void BeginScene() override
    {
    }

    void EndScene() override
    {
    }

    void Execute() override
    {
        RenderPipeline::Get()->GetUniformBuffer("SceneData")->Bind(0);
        RenderPipeline::Get()->GetUniformBuffer("PointLightData")->Bind(1);
        RenderPipeline::Get()->GetUniformBuffer("SpotLightData")->Bind(2);

        auto *scene = m_scene;
        auto &sceneData = scene->GetData();

        auto vaoView = scene->View<TransformComponent, VAOComponent, MaterialComponent>();
        for (auto entity : vaoView)
        {
            const auto &transform = vaoView.get<TransformComponent>(entity);
            const auto &vao = vaoView.get<VAOComponent>(entity);
            const auto &material = vaoView.get<MaterialComponent>(entity);

            glm::mat4 model = transform.GetModelMatrix();

            material.MaterialInstance->SetUniformMatrix4f("u_Model", model);
            material.MaterialInstance->SetUniformMatrix4f("u_View", sceneData.CameraData.View);
            material.MaterialInstance->SetUniformMatrix4f("u_Projection", sceneData.CameraData.Projection);

            material.MaterialInstance->Bind();

            vao.Render();
        }

        auto meshView = scene->View<TransformComponent, MeshComponent, MaterialComponent>();
        for (auto entity : meshView)
        {

            const auto &transform = meshView.get<TransformComponent>(entity);
            const auto &mesh = meshView.get<MeshComponent>(entity);
            const auto &material = meshView.get<MaterialComponent>(entity);

            glm::mat4 model = transform.GetModelMatrix();

            material.MaterialInstance->SetUniformMatrix4f("u_Model", model);
            material.MaterialInstance->SetUniformMatrix4f("u_View", sceneData.CameraData.View);
            material.MaterialInstance->SetUniformMatrix4f("u_Projection", sceneData.CameraData.Projection);

            material.MaterialInstance->Bind();
            mesh.Render();
        }
    }
};

} // namespace Doodle