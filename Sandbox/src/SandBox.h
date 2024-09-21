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
#include "Model.h"
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
        auto nanosuitModel = Model::Create("assets/models/nanosuit/nanosuit.obj");
        auto nanosuit = m_scene->CreateEntityFromModel(nanosuitModel);
        nanosuit.GetComponent<TransformComponent>().SetLocalScale(0.1);
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
