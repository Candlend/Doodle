#pragma once

#include "pch.h"

#include "Singleton.h"
#include "Core.h"
#include "ApplicationRunner.h"
#include "EventManager.h"
#include "LayerStack.h"
#include "ApplicationEvent.h"
#include "Window.h"

namespace RhyEngine
{

class RHY_API Application
{
    friend class ApplicationRunner;
public:
    Application();
    virtual ~Application();

    void Run();
    void OnEvent(BaseEvent &e);
    bool OnWindowCloseEvent(WindowCloseEvent &e);

    void PushLayer(BaseLayer *layer);
    void PushOverlay(BaseLayer *layer);

    inline Window &GetWindow()
    {
        return *m_window;
    };

    inline static Application &Get()
    {
        return ApplicationRunner::Get().GetCurrentApp();
    }

private:
    std::unique_ptr<Window> m_window;
    bool m_running = true;
    LayerStack m_layerStack;
    EventManager m_eventManager;
};

} // namespace RhyEngine
