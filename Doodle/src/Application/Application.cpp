#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "Application.h"
#include "ApplicationEvent.h"
#include "ApplicationRunner.h"
#include "EventManager.h"
#include "ImGuiBuilder.h"
#include "Log.h"
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

Application::Application()
{
    EventManager::Get()->AddListener(this, &Application::OnWindowClose);
    EventManager::Get()->AddListener(this, &Application::OnWindowRefresh);
    EventManager::Get()->AddListener(this, &Application::OnWindowMove);

    EventManager::Get()->AddListener<AppUpdateEvent>(this, &Application::BeforeUpdate, ExecutionOrder::First + 1);
    EventManager::Get()->AddListener<AppUpdateEvent>(this, &Application::AfterUpdate, ExecutionOrder::Last - 1);
    EventManager::Get()->AddListener<AppLayoutEvent>(this, &Application::BeforeLayout, ExecutionOrder::First + 1);
    EventManager::Get()->AddListener<AppLayoutEvent>(this, &Application::AfterLayout, ExecutionOrder::Last - 1);
    EventManager::Get()->AddListener<AppRenderEvent>(this, &Application::BeforeRender, ExecutionOrder::First + 1);
    EventManager::Get()->AddListener<AppRenderEvent>(this, &Application::AfterRender, ExecutionOrder::Last - 1);
}

Application::~Application()
{
}

void Application::Initialize()
{
    DOO_CORE_INFO("Application initialized");
    Renderer::Get()->Initialize();
    ImGuiBuilder::Get()->Initialize();
}

void Application::Deinitialize()
{
    DOO_CORE_INFO("Application deinitialized");
    Renderer::Get()->Deinitialize();
    ImGuiBuilder::Get()->Deinitialize();
}

void Application::Run()
{
    while (m_running)
    {
        Time::Update();
        auto window = ApplicationRunner::GetWindow();
        window->PollEvents();
        EventManager::Get()->Dispatch<AppUpdateEvent>();
        EventManager::Get()->Dispatch<AppRenderEvent>();
        EventManager::Get()->Dispatch<AppLayoutEvent>();
        window->SwapBuffers();
    }
}

bool Application::OnWindowClose(WindowCloseEvent & /*e*/)
{
    m_running = false;
    return true;
}

bool Application::OnWindowRefresh(WindowRefreshEvent & /*e*/)
{
    Time::Freeze();
    auto window = ApplicationRunner::GetWindow();
    EventManager::Get()->Dispatch<AppRenderEvent>();
    EventManager::Get()->Dispatch<AppLayoutEvent>();
    window->SwapBuffers();
    return false;
}

bool Application::OnWindowMove(WindowMoveEvent & /*e*/)
{
    Time::Freeze();
    return false;
}

} // namespace Doodle
