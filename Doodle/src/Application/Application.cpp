#include <glad/glad.h>

#include "Application.h"
#include "ApplicationEvent.h"
#include "ApplicationRunner.h"
#include "EventManager.h"
#include "ImGuiManager.h"
#include "Log.h"
#include "Module.h"
#include "Renderer.h"
#include "imgui.h"


namespace Doodle
{


void Application::Initialize()
{
    DOO_CORE_TRACE("Application Start");

    EventManager::Get().AddListener(this, &Application::OnWindowCloseEvent);
    EventManager::Get().AddListener(this, &Application::OnAppLayoutEvent);
    EventManager::Get().AddListener(this, &Application::OnWindowResizeEvent);
    
    Renderer::Get().Initialize();
    ImGuiManager::Get().Initialize();
    
    for (auto *module : Module::GetModules())
    {
        module->Initialize();
    }
}

void Application::Deinitialize()
{
    for (auto *module : Module::GetModules())
    {
        module->Deinitialize();
    }

    Renderer::Get().Deinitialize();
    ImGuiManager::Get().Deinitialize();

    EventManager::Get().RemoveListener(this, &Application::OnAppLayoutEvent);
    EventManager::Get().RemoveListener(this, &Application::OnWindowCloseEvent);
    EventManager::Get().RemoveListener(this, &Application::OnWindowResizeEvent);
    DOO_CORE_TRACE("Application End");
}

void Application::OnUpdate()
{
    auto window = m_window.lock();
    window->BeginFrame();
    for (auto *module : Module::GetModules())
    {
        module->OnUpdate();
    }
    Renderer::Get().WaitAndRender();
    ImGuiManager::Get().DrawLayout();
    window->EndFrame();
}

void Application::Run()
{
    while (m_running)
    {
        OnUpdate();
    }
}

bool Application::OnWindowCloseEvent(WindowCloseEvent & /*e*/)
{
    m_running = false;
    return true;
}

void Application::OnLayout()
{
    for (auto *module : Module::GetModules())
    {
        module->OnLayout();
    }
}

bool Application::OnWindowResizeEvent(WindowResizeEvent & /*e*/)
{
    OnUpdate();
    return false;
}

bool Application::OnAppLayoutEvent(AppLayoutEvent & /*e*/)
{
    OnLayout();
    return false;
}

} // namespace Doodle
