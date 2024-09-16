#pragma once

#include "pch.h"

#include "Component.h"
#include "RenderPass.h"
#include "RenderPipeline.h"
#include "Texture.h"


namespace Doodle
{

class DOO_API ShadingPass : public RenderPass
{
public:
    ShadingPass(const RenderPassSpecification &specification) : RenderPass(specification)
    {
        TextureParams params;
        params.Wrap = TextureWrap::ClampToEdge;
        m_brdfLUT = Texture2D::Create("assets/textures/brdfLUT.png", params);
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
        auto irradienceMap = sceneData.EnvironmentData.IrradianceMap;
        auto prefilterMap = sceneData.EnvironmentData.RadianceMap;

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
            material.MaterialInstance->SetUniformTexture("u_IrradianceMap", irradienceMap);
            material.MaterialInstance->SetUniformTexture("u_PrefilterMap", prefilterMap);
            material.MaterialInstance->SetUniformTexture("u_brdfLUT", m_brdfLUT);

            material.MaterialInstance->Bind();
            vao.Render();
            material.MaterialInstance->Unbind();
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
            material.MaterialInstance->SetUniformTexture("u_IrradianceMap", irradienceMap);
            material.MaterialInstance->SetUniformTexture("u_PrefilterMap", prefilterMap);
            material.MaterialInstance->SetUniformTexture("u_BrdfLUT", m_brdfLUT);

            material.MaterialInstance->Bind();
            mesh.Render();
            material.MaterialInstance->Unbind();
        }
    }

private:
    std::shared_ptr<Texture2D> m_brdfLUT;
};

} // namespace Doodle