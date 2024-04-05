#pragma once

#include "ImGuiManager.h"
#include "pch.h"

#include "ApplicationEvent.h"
#include "Core.h"
#include "EventManager.h"
#include "Singleton.h"
#include "Window.h"
#include <memory>


namespace RhyEngine
{
class ApplicationRunner;

class RHY_API Application
{
    friend class ApplicationRunner;

public:
    void Initialize();
    void Deinitialize();

    void Run();
    bool OnWindowCloseEvent(WindowCloseEvent &e);
    static Application &Get();


private:
    std::weak_ptr<Window> m_window;
    bool m_running = true;
};

} // namespace RhyEngine
