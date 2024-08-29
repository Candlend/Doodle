#pragma once

#include <imgui.h>

#include <ImGuizmo.h>
#include <glm/gtc/matrix_transform.hpp>

#include <Doodle.h>

#include "CameraController.h"

using namespace Doodle;

class SceneHierarchy : public RegisterModule<SceneHierarchy>
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

        m_activeScene = SceneManager::Get().GetActiveScene();

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
        mainCamera->GetComponent<Transform>().Position = glm::vec3(0.f, 0.f, 3.f);
        mainCamera->AddComponent<NativeScript>().Bind<CameraController>();

        auto directionalLight = m_activeScene->CreateEntity("DirectionalLight");
        auto &light = directionalLight->AddComponent<DirectionalLightComponent>();
        light.Intensity = 1.0f;
    }

    void OnUpdate() override
    {
        Renderer::UseWireframe(m_useWireframe);
        Renderer::Clear();

        auto cerberus = m_activeScene->GetEntity("Cerberus");
        auto &transform = cerberus->GetComponent<Transform>();
        transform.Rotation.y += 100.f * Application::Time::GetDeltaTime();
        m_activeScene->OnUpdate();
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