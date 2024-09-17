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

#define SET_UNIFORMS()                                                                                                 \
    material.MaterialInstance->SetUniformMatrix4f("u_Model", model);                                                   \
    material.MaterialInstance->SetUniformMatrix4f("u_View", sceneData.CameraData.View);                                \
    material.MaterialInstance->SetUniformMatrix4f("u_Projection", sceneData.CameraData.Projection);                    \
    material.MaterialInstance->SetUniformTexture("u_IrradianceMap", irradienceMap->GetTextureHandle());                \
    material.MaterialInstance->SetUniformTexture("u_PrefilterMap", prefilterMap->GetTextureHandle());                  \
    material.MaterialInstance->SetUniformTexture("u_BrdfLUT", m_brdfLUT->GetTextureHandle());                          \
    material.MaterialInstance->SetUniformTexture("u_LTC1", m_ltc1->GetTextureHandle());                                \
    material.MaterialInstance->SetUniformTexture("u_LTC2", m_ltc2->GetTextureHandle());                                \
    material.MaterialInstance->SetUniformMatrix4f("u_LightSpaceMatrix", lightSpaceMatrix);                             \
    material.MaterialInstance->SetUniformTexture("u_ShadowMap", shadowMap->GetDepthAttachmentTextureHandle());         \
    material.MaterialInstance->SetUniform1f("u_ShadowBias", sceneData.ShadowBias);                                     \
    material.MaterialInstance->SetUniform1f("u_ShadowNormalBias", sceneData.ShadowNormalBias);

class DOO_API ShadingPass : public RenderPass
{
public:
    ShadingPass(const RenderPassSpecification &specification) : RenderPass(specification)
    {
        TextureParams params;
        params.Wrap = TextureWrap::ClampToEdge;
        m_brdfLUT = Texture2D::Create("assets/textures/brdfLUT.png", params);

        params.Wrap = TextureWrap::ClampToEdge;
        params.Filter = TextureFilter::Linear;
        Buffer ltc1Buffer = Buffer::Copy(LTC1, sizeof(LTC1));
        m_ltc1 = Texture2D::Create(ltc1Buffer, params);
        Buffer ltc2Buffer = Buffer::Copy(LTC2, sizeof(LTC2));
        m_ltc2 = Texture2D::Create(ltc2Buffer, params);
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

        Renderer::SetDepthTest(DepthTestType::LessEqual);
        auto vaoView = scene->View<TransformComponent, VAOComponent, MaterialComponent>();
        for (auto entity : vaoView)
        {
            const auto &transform = vaoView.get<TransformComponent>(entity);
            const auto &vao = vaoView.get<VAOComponent>(entity);
            const auto &material = vaoView.get<MaterialComponent>(entity);

            glm::mat4 model = transform.GetModelMatrix();

            SET_UNIFORMS();

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

            SET_UNIFORMS();

            material.MaterialInstance->Bind();
            mesh.Render();
            material.MaterialInstance->Unbind();
        }
        Renderer::SetDepthTest(DepthTestType::Less);
    }

private:
    std::shared_ptr<Texture2D> m_brdfLUT;
    std::shared_ptr<Texture2D> m_ltc1;
    std::shared_ptr<Texture2D> m_ltc2;
};

} // namespace Doodle