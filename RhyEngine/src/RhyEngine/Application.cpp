#include <glad/glad.h>

#include "Application.h"
#include "Log.h"
#include "RhyEngine/Event/ApplicationEvent.h"
#include "RhyEngine/Event/Event.h"
#include "RhyEngine/Event/EventManager.h"
#include "Window.h"

namespace RhyEngine
{

Application::Application()
{
    m_window = std::unique_ptr<Window>(Window::Create());
    m_window->SetEventCallback([this](BaseEvent &e) { OnEvent(e); });
    EventManager::GetInstance().AddListener(EventType::WindowClose, [this](BaseEvent & /*e*/) { return Quit(); });
}

void Application::Run()
{
    while (m_running)
    {
        glClearColor(1, 0, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        m_window->OnUpdate();
    }
}

bool Application::Quit()
{
    m_running = false;
    return true;
}

void Application::OnEvent(BaseEvent &e)
{
    RHY_CORE_TRACE(e.ToString());
    EventManager::GetInstance().Dispatch(e);
}
} // namespace RhyEngine