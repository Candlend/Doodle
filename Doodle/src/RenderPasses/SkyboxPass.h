#pragma once

#include "RendererAPI.h"
#include "ShaderLibrary.h"
#include "pch.h"
#include <glad/glad.h>

#include "Component.h"
#include "Material.h"
#include "RenderPass.h"
#include "RenderPipeline.h"

namespace Doodle
{

class DOO_API SkyboxPass : public RenderPass
{
public:
    SkyboxPass(const RenderPassSpecification &specification) : RenderPass(specification)
    {
        m_shader = ShaderLibrary::Get()->GetShader("skybox");
        m_mesh = Mesh::GetCube();

        std::array<std::string, 6> faces = {
            "assets/textures/skybox/right.jpg",  "assets/textures/skybox/left.jpg",  "assets/textures/skybox/top.jpg",
            "assets/textures/skybox/bottom.jpg", "assets/textures/skybox/front.jpg", "assets/textures/skybox/back.jpg",
        };
        TextureSpecification spec;
        spec.Format = TextureFormat::SRGB8;
        m_defaultTextureCube = TextureCube::Create(faces, spec);
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
        glm::mat4 view = sceneData.CameraData.View;
        view[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        glm::mat4 projection = sceneData.CameraData.Projection;

        EnvironmentData &environment = sceneData.EnvironmentData;
        auto skyboxTexture = environment.RadianceMap;

        if (skyboxTexture == nullptr)
        {
            skyboxTexture = m_defaultTextureCube;
        }

        Renderer::SetDepthTest(DepthTestType::LessEqual);
        Renderer::SetCullFace(CullFaceType::Front);
        m_shader->Bind();
        m_shader->SetUniformTexture("u_Skybox", skyboxTexture);
        m_shader->SetUniform1f("u_Intensity", environment.Intensity);
        m_shader->SetUniform1f("u_Rotation", environment.Rotation);
        m_shader->SetUniformMatrix4f("u_View", view);
        m_shader->SetUniformMatrix4f("u_Projection", projection);

        m_mesh->Render();
        m_shader->Unbind();
        Renderer::SetDepthTest(DepthTestType::Less);
        Renderer::SetCullFace(CullFaceType::Back);
    }

    void OnLayout() override
    {
        auto &sceneData = m_scene->GetData();
        ImGui::DragFloat("Intensity", &sceneData.EnvironmentData.Intensity, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("Rotation", &sceneData.EnvironmentData.Rotation, 0.1f, 0.0f, 360.0f);
    }

private:
    std::shared_ptr<Shader> m_shader;
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<TextureCube> m_textureCube;
    std::shared_ptr<TextureCube> m_defaultTextureCube;
};

} // namespace Doodle