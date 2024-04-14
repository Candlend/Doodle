#pragma once

#include "Application.h"
#include <RhyEngine.h>

using namespace RhyEngine;

class Sandbox : public Application
{
public:
    void Initialize() override {
        Application::Initialize();
        ActivateImGuiContext();
        RHY_INFO("Sandbox initialized");
    }

    void Deinitialize() override {
        Application::Deinitialize();
        RHY_INFO("Sandbox deinitialized");
    }

    void OnLayout() override {
        ImGui::ShowDemoWindow();

        ImGuiUtils::ImGuiScope scope;
        if (scope.IsOpened()) {
            ImGui::Text("Hello, world!");
            ImGuiUtils::ChildWindowScope childWindow("ChildWindow", ImVec2(200, 200), ImGuiWindowFlags_None);
            if (childWindow.IsOpened()) {
                ImGui::Text("Hello, child window!");
            }
        }
    }
};
