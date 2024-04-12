#include <glad/glad.h>

#include "Application.h"
#include "ApplicationRunner.h"
#include "EventManager.h"
#include "ImGuiManager.h"
#include "Log.h"
#include "imgui.h"

namespace RhyEngine
{

void Application::Initialize()
{
    RHY_CORE_TRACE("Application Start");
    EventManager::Get().AddListener(this, &Application::OnWindowCloseEvent);
    ImGuiManager::Get().Initialize();
    EventManager::Get().AddListener(this, &Application::OnAppLayoutEvent);
}

void Application::Deinitialize()
{
    EventManager::Get().AddListener(this, &Application::OnAppLayoutEvent);
    ImGuiManager::Get().Deinitialize();
    EventManager::Get().RemoveListener(this, &Application::OnWindowCloseEvent);
    RHY_CORE_TRACE("Application End");
}

void Application::Run()
{
    while (m_running)
    {
        glClearColor(1, 0, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGuiManager::Get().DrawLayout();
        m_window.lock()->OnUpdate();
    }
}

bool Application::OnWindowCloseEvent(WindowCloseEvent & /*e*/)
{
    m_running = false;
    return true;
}

void Application::OnLayout()
{
    // ImGui::SetCurrentContext(ImGuiManager::Get().GetContext());
    RHY_DEBUG("OnLayout Start");
    ImGui::ShowDemoWindow();
    RHY_DEBUG("OnLayout End");
}

bool Application::OnAppLayoutEvent(AppLayoutEvent &  /*e*/)
{
    OnLayout();
    return false;
}

} // namespace RhyEngine
