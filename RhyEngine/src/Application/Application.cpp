#include <glad/glad.h>

#include "Application.h"
#include "ApplicationRunner.h"
#include "EventManager.h"
#include "ImGuiManager.h"

namespace RhyEngine
{

void Application::Initialize()
{
    RHY_CORE_TRACE("Application Start");
    EventManager::Get().AddListener(this, &Application::OnWindowCloseEvent);
    ImGuiManager::Get().Initialize();
}

void Application::Deinitialize()
{
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

} // namespace RhyEngine
