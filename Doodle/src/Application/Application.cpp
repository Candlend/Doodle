#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "Application.h"
#include "ApplicationEvent.h"
#include "ApplicationRunner.h"
#include "EventManager.h"
#include "ImGuiBuilder.h"
#include "Input.h"
#include "Log.h"
#include "Renderer.h"
#include "Window.h"

namespace Doodle
{

float Application::Time::s_FPS = 0.0f;
float Application::Time::s_Time = 0.0f;
float Application::Time::s_DeltaTime = 0.0f;
float Application::Time::s_LastFrameTime = 0.0f;
uint64_t Application::Time::s_FrameCount = 0;

float Application::Time::GetTime()
{
    return s_Time;
}

float Application::Time::GetDeltaTime()
{
    return s_DeltaTime;
}

uint64_t Application::Time::GetFrameCount()
{
    return s_FrameCount;
}

void Application::Time::Update()
{
    s_Time = static_cast<float>(glfwGetTime());
    s_FrameCount++;
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
    Input::Get()->Initialize();
    Renderer::Get()->Initialize();
    ImGuiBuilder::Get()->Initialize();
}

void Application::Deinitialize()
{
    DOO_CORE_INFO("Application deinitialized");
    Renderer::Get()->Deinitialize();
    ImGuiBuilder::Get()->Deinitialize();
    Input::Get()->Deinitialize();
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

bool Application::OnEvent(WindowCloseEvent & /*e*/)
{
    m_running = false;
    return true;
}

bool Application::OnEvent(WindowRefreshEvent & /*e*/)
{
    Time::Freeze();
    return false;
}

bool Application::OnEvent(WindowMoveEvent & /*e*/)
{
    Time::Freeze();
    return false;
}

} // namespace Doodle
