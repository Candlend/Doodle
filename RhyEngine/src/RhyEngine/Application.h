#pragma once

#include "Common/Singleton.h"
#include "Core.h"
#include "Event/EventManager.h"
#include "Layer/LayerStack.h"
#include "RhyEngine/Event/ApplicationEvent.h"
#include "Window.h"


namespace RhyEngine
{

class RHY_API Application : public Singleton<Application>
{

public:
    Application();
    virtual ~Application() = default;

    void Run();
    void OnEvent(BaseEvent &e);
    bool Quit();

    void PushLayer(BaseLayer *layer);
    void PushOverlay(BaseLayer *layer);

    inline Window &GetWindow()
    {
        return *m_window;
    }

private:
    std::unique_ptr<Window> m_window;
    bool m_running = true;
    LayerStack m_layerStack;
    EventManager m_eventManager;
};

// To be defined in CLIENT
Application *CreateApplication();

} // namespace RhyEngine