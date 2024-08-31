#pragma once

#include "CameraController.h"
#include "Component.h"
#include "Log.h"
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
        return std::make_shared<Sandbox>();
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
        BuildScene();
    }

    void BuildScene()
    {

        ShaderLibrary::Get()->LoadShader("Test", "assets/shaders/shader.glsl");
        TextureParams params;
        params.Format = TextureFormat::SRGB8ALPHA8;
        auto albedoTexture = Texture2D::Create("assets/textures/cerberus/cerberus_A.png", params);
        auto normalTexture = Texture2D::Create("assets/textures/cerberus/cerberus_N.png", params);
        auto metallicTexture = Texture2D::Create("assets/textures/cerberus/cerberus_M.png", params);
        auto roughnessTexture = Texture2D::Create("assets/textures/cerberus/cerberus_R.png", params);

        auto material = Material::Create("Test");

        material->SetUniformTexture("u_AlbedoTexture", albedoTexture);
        material->SetUniformTexture("u_NormalTexture", normalTexture);
        material->SetUniformTexture("u_MetallicTexture", metallicTexture);
        material->SetUniformTexture("u_RoughnessTexture", roughnessTexture);

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

        // 创建方向光
        auto directionalLight = m_scene->CreateEntity("DirectionalLight");
        auto &dirLight = directionalLight->AddComponent<DirectionalLightComponent>();
        dirLight.Intensity = 0.5f;
        dirLight.Radiance = glm::vec3(1.0f, 1.0f, 1.0f); // 白色光

        // 设置方向光的 TransformComponent
        auto &transform = directionalLight->GetComponent<TransformComponent>();
        transform.Position = glm::vec3(0.0f, 5.0f, 0.0f);                               // 位置
        transform.Rotation = glm::vec3(glm::radians(45.0f), glm::radians(45.0f), 0.0f); // 旋转（以弧度为单位）

        // 创建点光源
        auto pointLight = m_scene->CreateEntity("PointLight");
        auto &pLight = pointLight->AddComponent<PointLightComponent>();
        pLight.Intensity = 0.8f;
        pLight.Radiance = glm::vec3(1.0f, 0.5f, 0.5f);                                         // 暖色光
        pointLight->GetComponent<TransformComponent>().Position = glm::vec3(2.0f, 1.0f, 0.0f); // 光源位置
        pLight.Radius = 5.0f;                                                                  // 光照半径
        pLight.Falloff = 1.0f;                                                                 // 衰减系数

        // 创建聚光灯
        auto spotLight = m_scene->CreateEntity("SpotLight");
        auto &sLight = spotLight->AddComponent<SpotLightComponent>();
        sLight.Intensity = 1.2f;
        sLight.Radiance = glm::vec3(0.5f, 0.5f, 1.0f);                                        // 冷色光
        spotLight->GetComponent<TransformComponent>().Position = glm::vec3(0.0f, 2.0f, 0.0f); // 光源位置
        sLight.Angle = glm::radians(30.0f);                                                   // 聚光灯的角度
        sLight.Range = 10.0f;                                                                 // 光照范围
        sLight.Falloff = 1.0f;                                                                // 衰减系数
        auto &sTransform = spotLight->GetComponent<TransformComponent>();
        sTransform.Position = glm::vec3(0.0f, 2.0f, 0.0f);                // 位置
        sTransform.Rotation = glm::vec3(glm::radians(90.0f), 0.0f, 0.0f); // 旋转（以弧度为单位）
    }

    void BeforeUpdate() override
    {
        auto cerberus = m_scene->FindEntity("Cerberus");
        cerberus->GetComponent<TransformComponent>().Rotation.y += 100.f * Time::GetDeltaTime();
        auto directionalLight = m_scene->FindEntity("DirectionalLight");
        directionalLight->GetComponent<TransformComponent>().Rotation.x += 50.f * Time::GetDeltaTime();
    }

    void Deinitialize() override
    {
        m_scene->EndScene();
        Application::Deinitialize();
    }

private:
    std::shared_ptr<Scene> m_scene;
};
