#pragma once

#include "CameraController.h"
#include "Component.h"
#include "DebugPanel.h"
#include "HierarchyPanel.h"
#include "ImGuiMenu.h"
#include "ImGuiUtils.Feature.h"
#include "InspectorPanel.h"
#include "Log.h"
#include "LogPanel.h"
#include "PanelManager.h"
#include "RenderSettingsPanel.h"
#include "Renderable.h"
#include "Texture.h"
#include "ViewportPanel.h"
#include "imgui.h"
#include <Doodle.h>
#include <array>
#include <cstdint>
#include <memory>
#include <string>


using namespace Doodle;

class Sandbox : public Application
{
public:
    static std::shared_ptr<Sandbox> Create()
    {
        return std::make_shared<Sandbox>();
    }

    void InitializeLayout()
    {
        auto fileMenuItem = std::make_shared<ParentMenuItem>("File");
        fileMenuItem->AddSubItem<ClickableMenuItem>(
            "Exit", []() { ApplicationRunner::Get()->GetApplication()->Shutdown(); }, "Ctrl+Q");

        m_mainMenu = std::make_shared<ImGuiMenu>();
        m_mainMenu->AddMenuItem<ParentMenuItem>(fileMenuItem);

        PanelManager::Get()->CreatePanel<LogPanel>();
        PanelManager::Get()->CreatePanel<HierarchyPanel>();
        PanelManager::Get()->CreatePanel<DebugPanel>();
        PanelManager::Get()->CreatePanel<InspectorPanel>();
        PanelManager::Get()->CreatePanel<ViewportPanel>();
        PanelManager::Get()->CreatePanel<RenderSettingsPanel>();

        auto layoutMenuItem = std::make_shared<ParentMenuItem>("Layout");
        for (const auto &[name, panel] : PanelManager::Get()->GetPanels())
        {
            layoutMenuItem->AddSubItem<SelectableMenuItem>(
                name, [panel](bool selected) { panel->SetOpened(selected); }, panel->GetShortcut(),
                [panel]() { return panel->IsOpened(); });
        }
        m_mainMenu->AddMenuItem<ParentMenuItem>(layoutMenuItem);
        m_mainMenu->RegisterShortcuts();
    }

    void Initialize() override
    {
        Application::Initialize();
        ActivateImGuiContext();
        ShaderLibrary::Get()->LoadShadersFromDirectory("assets/shaders");
        InitializeLayout();
        m_scene = SceneManager::Get()->CreateScene("Main");
        m_scene->LoadEnvironment("assets/envs/pink_sunrise_4k.hdr");
        BuildScene();
        m_scene->BeginScene();
    }

    void BeforeLayout() override
    {
        ImGui::BeginMainMenuBar();
        m_mainMenu->OnLayout();
        ImGui::EndMainMenuBar();
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
    }

    void BuildScene()
    {
        auto material = StandardMaterial::Create();

        TextureParams params;
        params.Format = TextureFormat::SRGB8;
        auto cerberus = m_scene->CreateEntity("Cerberus");
        cerberus.AddComponent<MeshComponent>("assets/models/cerberus.fbx");
        cerberus.AddComponent<MaterialComponent>(material);
        cerberus.GetComponent<TransformComponent>().SetLocalScale(0.01f);
        auto &cerberusMat = cerberus.GetComponent<MaterialComponent>().MaterialInstance;
        cerberusMat->LoadAlbedoTexture("assets/textures/cerberus/cerberus_A.png", params);
        cerberusMat->LoadNormalTexture("assets/textures/cerberus/cerberus_N.png");
        cerberusMat->LoadMetallicTexture("assets/textures/cerberus/cerberus_M.png");
        cerberusMat->LoadRoughnessTexture("assets/textures/cerberus/cerberus_R.png");

        auto cube = m_scene->CreateEntity("Cube");
        cube.AddComponent<MeshComponent>(Mesh::GetCube());
        cube.AddComponent<MaterialComponent>(material);
        cube.GetComponent<TransformComponent>().SetLocalPosition(glm::vec3(2.f, 0.f, 0.f));
        auto &cubeMat = cube.GetComponent<MaterialComponent>().MaterialInstance;
        cubeMat->LoadAlbedoTexture("assets/textures/test_cube/test_cube_diffuse.tga", params);

        auto sphere = m_scene->CreateEntity("Sphere");
        sphere.SetParent(cube);
        sphere.AddComponent<MeshComponent>("assets/models/sphere.obj");
        sphere.AddComponent<MaterialComponent>(material);
        sphere.GetComponent<TransformComponent>().SetLocalPosition(glm::vec3(-2.f, 0.f, 0.f));
        auto &sphereMat = sphere.GetComponent<MaterialComponent>().MaterialInstance;
        sphereMat->LoadAlbedoTexture("assets/textures/rusted_iron/base_color.tga", params);
        sphereMat->LoadNormalTexture("assets/textures/rusted_iron/normal.tga");
        sphereMat->LoadMetallicTexture("assets/textures/rusted_iron/metallic.tga");
        sphereMat->LoadRoughnessTexture("assets/textures/rusted_iron/roughness.tga");

        auto suzanne = m_scene->CreateEntity("Suzanne");
        suzanne.SetParent(cube);
        suzanne.AddComponent<MeshComponent>("assets/models/suzanne.obj");
        suzanne.AddComponent<MaterialComponent>(material);
        suzanne.GetComponent<TransformComponent>().SetLocalPosition(glm::vec3(0.f, 0.f, 2.f));

        auto cutFish = m_scene->CreateEntity("CutFish");
        cutFish.SetParent(suzanne);
        cutFish.AddComponent<MeshComponent>("assets/models/cut_fish.obj");
        cutFish.AddComponent<MaterialComponent>(material);
        cutFish.GetComponent<TransformComponent>().SetLocalPosition(glm::vec3(0.f, 0.f, -5.f));
        auto &cutFishMat = cutFish.GetComponent<MaterialComponent>().MaterialInstance;
        cutFishMat->LoadAlbedoTexture("assets/textures/cut_fish/base_color.tga", params);
        cutFishMat->LoadNormalTexture("assets/textures/cut_fish/normal.tga");
        cutFishMat->LoadMetallicTexture("assets/textures/cut_fish/metallic.tga");
        cutFishMat->LoadRoughnessTexture("assets/textures/cut_fish/roughness.tga");

        auto mainCamera = m_scene->CreateEntity("MainCamera");
        mainCamera.AddComponent<CameraComponent>();
        mainCamera.GetComponent<TransformComponent>().SetLocalPosition(glm::vec3(0.f, 0.f, 3.f));
        mainCamera.AddComponent<CameraController>();

        // 创建方向光
        auto directionalLight = m_scene->CreateEntity("DirectionalLight");
        auto &dirLight = directionalLight.AddComponent<DirectionalLightComponent>();
        dirLight.Intensity = 1.0f;
        dirLight.Radiance = glm::vec3(1.0f, 0.9f, 0.8f); // 白色光

        // 设置方向光的 TransformComponent
        auto &transform = directionalLight.GetComponent<TransformComponent>();
        transform.SetLocalPosition(glm::vec3(0.0f, 5.0f, 0.0f));                               // 位置
        transform.SetLocalRotation(glm::vec3(glm::radians(45.0f), glm::radians(45.0f), 0.0f)); // 旋转（以弧度为单位）

        // 创建点光源
        auto pointLight = m_scene->CreateEntity("PointLight");
        auto &pLight = pointLight.AddComponent<PointLightComponent>();
        pLight.Intensity = 0.8f;
        pLight.Radiance = glm::vec3(0.5f, 1.0f, 0.8f);                                         // 暖色光
        pointLight.GetComponent<TransformComponent>().SetLocalPosition(glm::vec3(2.0f, 1.0f, 0.0f)); // 光源位置
        pLight.Radius = 5.0f;                                                                  // 光照半径
        pLight.Falloff = 1.0f;                                                                 // 衰减系数

        // 创建聚光灯
        auto spotLight = m_scene->CreateEntity("SpotLight");
        auto &sLight = spotLight.AddComponent<SpotLightComponent>();
        sLight.Intensity = 1.2f;
        sLight.Radiance = glm::vec3(0.5f, 0.5f, 1.0f);                                        // 冷色光
        spotLight.GetComponent<TransformComponent>().SetLocalPosition(glm::vec3(0.0f, 2.0f, 0.0f)); // 光源位置
        sLight.Angle = glm::radians(30.0f);                                                   // 聚光灯的角度
        sLight.Range = 10.0f;                                                                 // 光照范围
        sLight.Falloff = 1.0f;                                                                // 衰减系数
        auto &sTransform = spotLight.GetComponent<TransformComponent>();
        sTransform.SetLocalPosition(glm::vec3(0.0f, 2.0f, 0.0f));                // 位置
        sTransform.SetLocalRotation(glm::vec3(glm::radians(90.0f), 0.0f, 0.0f)); // 旋转（以弧度为单位）

        // 创建面光源
        auto areaLight = m_scene->CreateEntity("AreaLight");
        auto &aLight = areaLight.AddComponent<AreaLightComponent>();
        aLight.Intensity = 1.0f;
        aLight.Radiance = glm::vec3(1.0f, 1.0f, 1.0f);                                       // 白色光
        areaLight.GetComponent<TransformComponent>().SetLocalPosition(glm::vec3(1.0f, 1.0f, 1.0f)); // 光源位置
    }

    void BeforeUpdate() override
    {
    }

    void Deinitialize() override
    {
        m_scene->EndScene();
        Application::Deinitialize();
    }

private:
    std::shared_ptr<Scene> m_scene;
    std::shared_ptr<ImGuiMenu> m_mainMenu;
};
