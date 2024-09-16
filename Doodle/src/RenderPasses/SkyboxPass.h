#pragma once

#include "Material.h"
#include "RenderPipeline.h"
#include "SceneEnvironment.h"
#include "pch.h"

#include "Component.h"
#include "RenderPass.h"
#include <memory>

namespace Doodle
{

class DOO_API SkyboxPass : public RenderPass
{
public:
    SkyboxPass(const RenderPassSpecification &specification) : RenderPass(specification)
    {
        m_material = SkyboxMaterial::Create();
        m_mesh = Mesh::Create("assets/models/test_cube.obj");

        std::array<std::string, 6> faces = {
            "assets/textures/skybox/right.jpg",  "assets/textures/skybox/left.jpg",  "assets/textures/skybox/top.jpg",
            "assets/textures/skybox/bottom.jpg", "assets/textures/skybox/front.jpg", "assets/textures/skybox/back.jpg",
        };
        TextureParams params;
        params.Format = TextureFormat::SRGB8;
        m_defaultTextureCube = TextureCube::Create(faces, params);
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

        Environment &environment = sceneData.Environment;
        auto skyboxTexture = environment.RadianceMap;

        if (skyboxTexture == nullptr)
        {
            skyboxTexture = m_defaultTextureCube;
        }

        m_material->Bind();
        m_material->SetUniformTexture("u_Skybox", skyboxTexture);
        m_material->SetUniformMatrix4f("u_View", view);
        m_material->SetUniformMatrix4f("u_Projection", projection);

        m_mesh->Render();
        m_material->Unbind();
    }

private:
    std::shared_ptr<Material> m_material;
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<TextureCube> m_textureCube;
    std::shared_ptr<TextureCube> m_defaultTextureCube;
};

} // namespace Doodle