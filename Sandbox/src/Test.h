#pragma once

#include "Component.h"
#include "Material.h"
#include "VertexBuffer.h"
#include <Doodle.h>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

using namespace Doodle;

class Test : public RegisterModule<Test>
{
public:
    void Initialize() override
    {
        Renderer::SetClearColor(0.2f, 0.2f, 0.2f, 1.f);

        ShaderLibrary::Get().LoadShader("Test", "assets/shaders/shader.glsl");
        TextureParams params;
        params.Format = TextureFormat::SRGB8ALPHA8;
        auto texture2D = Texture2D::Create("assets/textures/cerberus/cerberus_A.png", params);

        auto material = Material::Create("Test");
        material->SetUniformTexture("u_Texture", texture2D);

        m_activeScene = Scene::Create();
        auto cerberus = m_activeScene->CreateEntity("Cerberus");
        cerberus->AddComponent<MeshComponent>("assets/models/cerberus.fbx");
        cerberus->AddComponent<MaterialComponent>(material);
        cerberus->GetComponent<Transform>().Scale = glm::vec3(0.01f);

        auto cube = m_activeScene->CreateEntity("Cube");
        cube->AddComponent<MeshComponent>("assets/models/Cube.fbx");
        cube->AddComponent<MaterialComponent>(material);
        cube->GetComponent<Transform>().Position = glm::vec3(0.5f, 0.f, 0.f);
        cube->GetComponent<Transform>().Scale = glm::vec3(0.01f);

        auto mainCamera = m_activeScene->CreateEntity("MainCamera");
        mainCamera->AddComponent<CameraComponent>();
    }

    void OnUpdate() override
    {
        Renderer::UseWireframe(m_useWireframe);
        Renderer::Clear();

        auto cerberus = m_activeScene->GetEntity("Cerberus");
        auto &transform = cerberus->GetComponent<Transform>();
        transform.Rotation.y += 100.f * Application::Time::GetDeltaTime();
        m_activeScene->Render();
    }

    void OnLayout() override
    {
        ImGui::ShowDemoWindow();

        ImGuiUtils::WindowScope scope("调试");
        if (scope.IsOpened())
        {
            ImGui::LabelText("时间", "%f", Application::Time::GetTime());
            ImGui::LabelText("帧率", "%f", Application::Time::GetFPS());
            ImGui::Checkbox("线框模式", &m_useWireframe);
        }
    }

    void Deinitialize() override
    {
    }

private:
    bool m_useWireframe = false;
    std::shared_ptr<Scene> m_activeScene;
};