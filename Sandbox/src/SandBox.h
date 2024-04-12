#pragma once

#include "Application.h"
#include <RhyEngine.h>

class Sandbox : public RhyEngine::Application
{
public:
    void Initialize() override {
        RhyEngine::Application::Initialize();
        RhyEngine::ActivateImGuiContext();
        RHY_INFO("Sandbox initialized");
    }

    void Deinitialize() override {
        RhyEngine::Application::Deinitialize();
        RHY_INFO("Sandbox deinitialized");
    }

    void OnLayout() override {
        ImGui::ShowDemoWindow();
    }
};
