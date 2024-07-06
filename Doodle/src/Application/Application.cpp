#include <glad/glad.h>
#include <glfw/glfw3.h>

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

float Application::Time::s_Time = 0.0f;
float Application::Time::s_DeltaTime = 0.0f;
float Application::Time::s_LastFrameTime = 0.0f;

float Application::Time::GetTime()
{
    return s_Time;
}

float Application::Time::GetDeltaTime()
{
    return s_DeltaTime;
}

void Application::Time::Update()
{
    s_Time = static_cast<float>(glfwGetTime());
    s_DeltaTime = GetTime() - s_LastFrameTime;
    s_LastFrameTime = GetTime();
}

float Application::Time::GetFPS()
{
    return 1.0f / s_DeltaTime;
}

void Application::Initialize()
{
    EventManager::Get().AddListener(this, &Application::OnWindowCloseEvent);
    EventManager::Get().AddListener(this, &Application::OnAppLayoutEvent);
    EventManager::Get().AddListener(this, &Application::OnWindowResizeEvent);

    Renderer::Initialize();
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

    Renderer::Deinitialize();
    ImGuiManager::Get().Deinitialize();

    EventManager::Get().RemoveListener(this, &Application::OnAppLayoutEvent);
    EventManager::Get().RemoveListener(this, &Application::OnWindowCloseEvent);
    EventManager::Get().RemoveListener(this, &Application::OnWindowResizeEvent);
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
        Time::Update();
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
