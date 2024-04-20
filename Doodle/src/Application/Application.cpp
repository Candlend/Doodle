#include <glad/glad.h>

#include "Application.h"
#include "ApplicationEvent.h"
#include "ApplicationRunner.h"
#include "EventManager.h"
#include "ImGuiManager.h"
#include "Log.h"
#include "imgui.h"

namespace Doodle
{

void Application::Initialize()
{
    DOO_CORE_TRACE("Application Start");
    ImGuiManager::Get().Initialize();
    EventManager::Get().AddListener(this, &Application::OnWindowCloseEvent);
    EventManager::Get().AddListener(this, &Application::OnAppLayoutEvent);
    EventManager::Get().AddListener(this, &Application::OnWindowResizeEvent);
}

void Application::Deinitialize()
{
    EventManager::Get().RemoveListener(this, &Application::OnAppLayoutEvent);
    EventManager::Get().RemoveListener(this, &Application::OnWindowCloseEvent);
    EventManager::Get().RemoveListener(this, &Application::OnWindowResizeEvent);
    ImGuiManager::Get().Deinitialize();
    DOO_CORE_TRACE("Application End");
}

void Application::Run()
{
    while (m_running)
    {
        auto window = m_window.lock();
        window->BeginFrame();
        ImGuiManager::Get().DrawLayout();
        window->EndFrame();
    }
}

bool Application::OnWindowCloseEvent(WindowCloseEvent & /*e*/)
{
    m_running = false;
    return true;
}

void Application::OnLayout()
{
}

bool Application::OnWindowResizeEvent(WindowResizeEvent &  e)
{
    DOO_CORE_DEBUG(e.ToString());
    auto window = m_window.lock();
    window->BeginFrame();
    ImGuiManager::Get().DrawLayout();
    window->EndFrame();
    return false;
}

bool Application::OnAppLayoutEvent(AppLayoutEvent &  /*e*/)
{
    OnLayout();
    return false;
}

} // namespace Doodle
