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
    EventManager::Get()->AddListener(this, &Application::OnWindowCloseEvent);
    EventManager::Get()->AddListener(this, &Application::OnWindowRefreshEvent);
    EventManager::Get()->AddListener(this, &Application::OnWindowMoveEvent);

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
    Renderer::Get()->Initialize();
    ImGuiBuilder::Get()->Initialize();
}

void Application::Deinitialize()
{
    Renderer::Get()->Deinitialize();
    ImGuiBuilder::Get()->Deinitialize();
}

void Application::Run()
{
    while (m_running)
    {
        Time::Update();
        auto window = Window::Get();
        window->PollEvents();
        AppUpdateEvent updateEvent;
        EventManager::Get()->Dispatch(updateEvent);
        AppLayoutEvent layoutEvent;
        EventManager::Get()->Dispatch(layoutEvent);
        AppRenderEvent renderEvent;
        EventManager::Get()->Dispatch(renderEvent);
        window->SwapBuffers();
    }
}

bool Application::OnWindowCloseEvent(WindowCloseEvent & /*e*/)
{
    m_running = false;
    return true;
}

bool Application::OnWindowRefreshEvent(WindowRefreshEvent & /*e*/)
{
    Time::Freeze();
    auto window = Window::Get();
    AppUpdateEvent e;
    EventManager::Get()->Dispatch(e);
    window->SwapBuffers();
    return false;
}

bool Application::OnWindowMoveEvent(WindowMoveEvent & /*e*/)
{
    Time::Freeze();
    return false;
}

} // namespace Doodle
