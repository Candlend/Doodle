#pragma once

#include "Common/Singleton.h"
#include "Core.h"
#include "Event/EventManager.h"
#include "Layer/LayerStack.h"
#include "RhyEngine/Event/ApplicationEvent.h"
#include "Window.h"
#include <memory>

namespace RhyEngine
{

class RHY_API Application : std::enable_shared_from_this<Application>
{
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
    }
    inline static Application& GetInstance() { return *s_Instance; }

private:
    std::unique_ptr<Window> m_window;
    bool m_running = true;
    LayerStack m_layerStack;
    EventManager m_eventManager;
    static Application* s_Instance;
};

// To be defined in CLIENT
Application *CreateApplication();

} // namespace RhyEngine
