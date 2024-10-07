#pragma once

#include "pch.h"

#include "Component.h"
#include "LTCMatrix.h"
#include "RenderPass.h"
#include "RenderPipeline.h"
#include "Texture.h"
#include <memory>

namespace Doodle
{

class DOO_API ShadingPass : public RenderPass
{
public:
    ShadingPass(const RenderPassSpecification &specification) : RenderPass(specification)
    {
        TextureSpecification spec;
        spec.Wrap = TextureWrap::ClampToEdge;
        m_brdfLUT = Texture2D::Create("assets/textures/brdfLUT.png", spec);

        spec.Wrap = TextureWrap::ClampToEdge;
        spec.Filter = TextureFilter::Linear;
        Buffer ltc1Buffer = Buffer::Copy(LTC1, sizeof(LTC1));
        m_ltc1 = Texture2D::Create(ltc1Buffer, spec);
        Buffer ltc2Buffer = Buffer::Copy(LTC2, sizeof(LTC2));
        m_ltc2 = Texture2D::Create(ltc2Buffer, spec);
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
        RenderPipeline::Get()->GetUniformBuffer("AreaLightData")->Bind(3);

        auto *scene = m_scene;
        auto &sceneData = scene->GetData();
        auto irradienceMap = sceneData.EnvironmentData.IrradianceMap;
        auto prefilterMap = sceneData.EnvironmentData.RadianceMap;

        glm::mat4 lightSpaceMatrix = RenderPipeline::Get()->GetUniformMatrix4f("u_LightSpaceMatrix");
        std::shared_ptr<FrameBuffer> shadowMap = RenderPipeline::Get()->GetFrameBuffer("ShadowMap");
        std::shared_ptr<FrameBuffer> occlusionMap = RenderPipeline::Get()->GetFrameBuffer("OcclusionMap");
        auto targetFrameBuffer = GetSpecification().TargetFrameBuffer;

        Renderer::SetDepthTest(DepthTestType::LessEqual);

        auto meshView = scene->View<TransformComponent, MeshComponent, MaterialComponent>();
        for (auto entity : meshView)
        {

            const auto &transform = meshView.get<TransformComponent>(entity);
            const auto &mesh = meshView.get<MeshComponent>(entity);
            const auto &material = meshView.get<MaterialComponent>(entity);

            glm::mat4 model = transform.GetTransformMatrix();
            auto shader = material.Material->GetShader();

            shader->SetUniformMatrix4f("u_Model", model);
            shader->SetUniformMatrix4f("u_View", sceneData.CameraData.View);
            shader->SetUniformMatrix4f("u_Projection", sceneData.CameraData.Projection);
            shader->SetUniformTexture("u_IrradianceMap", irradienceMap);
            shader->SetUniformTexture("u_PrefilterMap", prefilterMap);
            shader->SetUniformTexture("u_BrdfLUT", m_brdfLUT);
            shader->SetUniformTexture("u_LTC1", m_ltc1);
            shader->SetUniformTexture("u_LTC2", m_ltc2);
            shader->SetUniformMatrix4f("u_LightSpaceMatrix", lightSpaceMatrix);
            shader->SetUniformTexture("u_ShadowMap", shadowMap->GetDepthAttachmentTextureHandle());
            shader->SetUniformTexture("u_OcclusionMap", occlusionMap->GetColorAttachmentTextureHandle(0));

            material.Material->Bind();
            mesh.Render();
            material.Material->Unbind();
        }
        Renderer::SetDepthTest(DepthTestType::Less);
    }

private:
    std::shared_ptr<Texture2D> m_brdfLUT;
    std::shared_ptr<Texture2D> m_ltc1;
    std::shared_ptr<Texture2D> m_ltc2;
};

} // namespace Doodle