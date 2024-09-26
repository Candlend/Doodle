#pragma once

#include <imgui.h>

#include "ApplicationRunner.h"
#include "DebugPanel.h"
#include "HierarchyPanel.h"
#include "ImGuiBuilder.h"
#include "ImGuiMenu.h"
#include "InspectorPanel.h"
#include "LogPanel.h"
#include "PanelManager.h"
#include "ProjectManager.h"
#include "RenderSettingsPanel.h"
#include "SceneManager.h"
#include "ShaderLibrary.h"
#include "ViewportPanel.h"

using namespace Doodle;

class DoodleEditor : public Application
{
public:
    static std::shared_ptr<DoodleEditor> Create()
    {
        return std::make_shared<DoodleEditor>();
    }

    void InitializeLayout()
    {
        auto fileMenuItem = std::make_shared<ParentMenuItem>("File");
        fileMenuItem->AddSubItem<ClickableMenuItem>(
            "New Project", []() { ProjectManager::Get()->CreateProject(); }, "Ctrl+N");
        fileMenuItem->AddSubItem<ClickableMenuItem>(
            "Open Project", []() { ProjectManager::Get()->LoadProject(); }, "Ctrl+O");
        fileMenuItem->AddSubItem<ClickableMenuItem>(
            "Save Scene", []() { SceneManager::Get()->SaveScene(); }, "Ctrl+S");
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
    }

    void BeforeLayout() override
    {
        ImGui::BeginMainMenuBar();
        m_mainMenu->OnLayout();
        ImGui::EndMainMenuBar();
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
    }

    void BeforeUpdate() override
    {
    }

    void Deinitialize() override
    {
        Application::Deinitialize();
    }

private:
    std::shared_ptr<Scene> m_scene;
    std::shared_ptr<ImGuiMenu> m_mainMenu;
};
