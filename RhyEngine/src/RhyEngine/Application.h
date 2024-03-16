#pragma once

#include "Common/Singleton.h"
#include "Core.h"
#include "Window.h"
#include "RhyEngine/Event/ApplicationEvent.h"

namespace RhyEngine
{

class RHY_API Application : Singleton<Application>
{
public:
    Application();

    virtual ~Application() = default;

    void Run();
    void OnEvent(BaseEvent& e);
    bool Quit();
private:

    std::unique_ptr<Window> m_window;
    bool m_running = true;
};

// To be defined in CLIENT
Application *CreateApplication();

} // namespace RhyEngine