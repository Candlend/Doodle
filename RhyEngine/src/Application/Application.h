#pragma once

#include "ImGuiManager.h"
#include "imgui.h"
#include "pch.h"

#include "ApplicationEvent.h"
#include "Core.h"
#include "EventManager.h"
#include "Singleton.h"
#include "Window.h"


namespace RhyEngine
{
class ApplicationRunner;

class RHY_API Application
{
    friend class ApplicationRunner;

public:
    static Application &Get();
    virtual void Initialize();
    virtual void Deinitialize();
    virtual void OnLayout();

protected:
    void Run();
    bool OnWindowCloseEvent(WindowCloseEvent &e);
    bool OnAppLayoutEvent(AppLayoutEvent &e);
    std::weak_ptr<Window> m_window;
    bool m_running = true;
};


} // namespace RhyEngine