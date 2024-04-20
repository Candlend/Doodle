#pragma once

#include "Application.h"
#include <Doodle.h>
#include "IconsFontAwesome6Pro.h"
#include "ImGuiUtils.h"

using namespace Doodle;

class Sandbox : public Application
{
public:
    void Initialize() override {
        Application::Initialize();
        ActivateImGuiContext();
        DOO_INFO("Sandbox initialized");
    }

    void Deinitialize() override {
        Application::Deinitialize();
        DOO_INFO("Sandbox deinitialized");
    }

    void OnLayout() override {
        ImGui::ShowDemoWindow();

        ImGuiUtils::WindowScope scope("测试");
        if (scope.IsOpened()) {
            ImGui::Text("%s 你好，世界！", ICON_FA_WINDOW);
            ImGuiUtils::ChildWindowScope childWindow("childWindow", ImVec2(200, 200), ImGuiChildFlags_Border);
            if (childWindow.IsOpened()) {
                ImGui::Text("Hello, child window!");
            }
        }
    }
};
