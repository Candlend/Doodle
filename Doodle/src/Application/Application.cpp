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
#include "Window.h"

namespace Doodle
{

float Application::Time::s_FPS = 0.0f;
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
    s_FPS = 1.0f / s_DeltaTime;
    s_LastFrameTime = GetTime();
}

void Application::Time::Freeze()
{
    s_DeltaTime = 0.0f;
    s_LastFrameTime = GetTime();
}

float Application::Time::GetFPS()
{
    return s_FPS;
}

std::shared_ptr<Window> Application::GetWindow()
{
    return m_window.lock();
}

void Application::Initialize()
{
    EventManager::Get().AddListener(this, &Application::OnWindowCloseEvent);
    EventManager::Get().AddListener(this, &Application::OnAppLayoutEvent);
    EventManager::Get().AddListener(this, &Application::OnWindowRefreshEvent);
    EventManager::Get().AddListener(this, &Application::OnWindowMoveEvent);

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
    EventManager::Get().RemoveListener(this, &Application::OnWindowRefreshEvent);
    EventManager::Get().RemoveListener(this, &Application::OnWindowMoveEvent);
}

void Application::OnUpdate()
{
    auto window = m_window.lock();
    for (auto *module : Module::GetModules())
    {
        module->OnUpdate();
    }
    Renderer::Get().WaitAndRender();
    ImGuiManager::Get().DrawLayout();
}

void Application::Run()
{
    while (m_running)
    {
        Time::Update();
        auto window = m_window.lock();
        window->PollEvents();
        OnUpdate();
        window->SwapBuffers();
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

bool Application::OnWindowRefreshEvent(WindowRefreshEvent & /*e*/)
{
    Time::Freeze();
    auto window = m_window.lock();
    OnUpdate();
    window->SwapBuffers();
    return false;
}

bool Application::OnWindowMoveEvent(WindowMoveEvent & /*e*/)
{
    Time::Freeze();
    return false;
}

bool Application::OnAppLayoutEvent(AppLayoutEvent & /*e*/)
{
    OnLayout();
    return false;
}

} // namespace Doodle
