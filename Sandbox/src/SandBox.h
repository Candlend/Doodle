#pragma once

#include "CameraController.h"
#include "LogPanel.h"
#include "PanelManager.h"
#include "SceneHierarchyPanel.h"
#include "imgui.h"
#include <Doodle.h>
#include <memory>

using namespace Doodle;

class Sandbox : public Application
{
public:
    static std::shared_ptr<Sandbox> Create()
    {
        auto sandbox = std::make_shared<Sandbox>();
        sandbox->SetInstance(sandbox);

        return sandbox;
    }

    void BeforeLayout() override
    {
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
        ImGui::ShowDemoWindow();
    }

    void Initialize() override
    {
        Application::Initialize();
        ActivateImGuiContext();
        PanelManager::Get()->CreatePanel<LogPanel>();
        PanelManager::Get()->CreatePanel<SceneHierarchyPanel>();
        m_scene = SceneManager::Get()->CreateScene("Main");
        m_scene->BeginScene();
        Renderer::SetClearColor(0.2f, 0.2f, 0.2f, 1.f);
        // BuildScene();
    }

    void BuildScene()
    {

        ShaderLibrary::Get()->LoadShader("Test", "assets/shaders/shader.glsl");
        TextureParams params;
        params.Format = TextureFormat::SRGB8ALPHA8;
        auto texture2D = Texture2D::Create("assets/textures/cerberus/cerberus_A.png", params);

        auto material = Material::Create("Test");
        material->SetUniformTexture("u_Texture", texture2D);

        auto cerberus = m_scene->CreateEntity("Cerberus");
        cerberus->AddComponent<MeshComponent>("assets/models/cerberus.fbx");
        cerberus->AddComponent<MaterialComponent>(material);
        cerberus->GetComponent<TransformComponent>().Scale = glm::vec3(0.01f);

        auto cube = m_scene->CreateEntity("Cube");
        cube->AddComponent<MeshComponent>("assets/models/Cube.fbx");
        cube->AddComponent<MaterialComponent>(material);
        cube->GetComponent<TransformComponent>().Position = glm::vec3(0.5f, 0.f, 0.f);
        cube->GetComponent<TransformComponent>().Scale = glm::vec3(0.01f);

        auto mainCamera = m_scene->CreateEntity("MainCamera");
        mainCamera->AddComponent<CameraComponent>();
        mainCamera->GetComponent<TransformComponent>().Position = glm::vec3(0.f, 0.f, 3.f);
        mainCamera->AddComponent<CameraController>();

        auto directionalLight = m_scene->CreateEntity("DirectionalLight");
        auto &light = directionalLight->AddComponent<DirectionalLightComponent>();
        light.Intensity = 1.0f;
    }

    void Deinitialize() override
    {
        m_scene->EndScene();
        Application::Deinitialize();
    }

private:
    std::shared_ptr<Scene> m_scene;
};
