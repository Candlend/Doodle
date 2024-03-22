#include <glad/glad.h>
#include <memory>

#include "Application.h"
#include "Event/ApplicationEvent.h"
#include "Log.h"
#include "RhyEngine/Event/ApplicationEvent.h"
#include "RhyEngine/Event/Event.h"
#include "RhyEngine/Event/EventManager.h"
#include "Window.h"

namespace RhyEngine
{

Application *Application::s_Instance = nullptr;

Application::Application()
{
    s_Instance = this;
    m_window = std::unique_ptr<Window>(Window::Create());
    m_window->SetEventCallback([this](BaseEvent &e) { OnEvent(e); });
    m_eventManager.AddListener(this, &Application::OnWindowCloseEvent);
}

Application::~Application()
{
    m_eventManager.RemoveListener(this, &Application::OnWindowCloseEvent);
}

void Application::Run()
{
    while (m_running)
    {
        glClearColor(1, 0, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        for (BaseLayer *layer : m_layerStack)
            layer->OnUpdate();

        m_window->OnUpdate();
    }
}

bool Application::OnWindowCloseEvent(WindowCloseEvent & /*e*/)
{
    m_running = false;
    return true;
}

void Application::OnEvent(BaseEvent &e)
{
    RHY_CORE_TRACE(e.ToString());
    m_eventManager.Dispatch(e);
    for (auto it = m_layerStack.end(); it != m_layerStack.begin();)
    {
        (*--it)->OnEvent(e);
        if (e.Handled)
            break;
    }
}

void Application::PushLayer(BaseLayer *layer)
{
    m_layerStack.PushLayer(layer);
    layer->OnAttach();
}

void Application::PushOverlay(BaseLayer *layer)
{
    m_layerStack.PushOverlay(layer);
    layer->OnAttach();
}

} // namespace RhyEngine
