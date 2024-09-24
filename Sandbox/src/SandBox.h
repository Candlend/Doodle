#pragma once

#include <imgui.h>

#include "DebugPanel.h"
#include "Doodle.h"
#include "HierarchyPanel.h"
#include "ImGuiBuilder.h"
#include "ImGuiMenu.h"
#include "InspectorPanel.h"
#include "LogPanel.h"
#include "Model.h"
#include "PanelManager.h"
#include "RenderSettingsPanel.h"
#include "Renderable.h"
#include "Texture.h"
#include "ViewportPanel.h"

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
        // auto nanosuitModel = Model::Create("assets/models/nanosuit/nanosuit.obj");
        // auto nanosuit = m_scene->CreateEntityFromModel(nanosuitModel);
        // nanosuit.GetComponent<TransformComponent>().SetLocalScale(0.1);

        // auto sponzaModel = Model::Create("assets/models/sponza_2/sponza.obj");
        // auto sponza = m_scene->CreateEntityFromModel(sponzaModel);
        // sponza.GetComponent<TransformComponent>().SetLocalScale(0.01);

        // auto suzanne = m_scene->CreateEntity("Suzanne");
        // suzanne.AddComponent<MeshComponent>(Mesh::Create("assets/models/suzanne.obj"));
        // suzanne.AddComponent<MaterialComponent>(StandardMaterial::Create());

        auto cutFish = m_scene->CreateEntity("CutFish");
        cutFish.AddComponent<MeshComponent>(Mesh::Create("assets/models/cut_fish.obj"));
        auto material = StandardMaterial::Create();
        TextureSpecification spec;
        spec.Format = TextureFormat::SRGB8ALPHA8;
        material->SetAlbedoTexture(Texture2D::Create("assets/textures/cut_fish/base_color.tga", spec));
        material->SetNormalTexture(Texture2D::Create("assets/textures/cut_fish/normal.tga"));
        material->SetMetallicTexture(Texture2D::Create("assets/textures/cut_fish/metallic.tga"));
        material->SetRoughnessTexture(Texture2D::Create("assets/textures/cut_fish/roughness.tga"));

        cutFish.AddComponent<MaterialComponent>(material);
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
