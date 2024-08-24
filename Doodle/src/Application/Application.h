#pragma once

#include "ImGuiManager.h"
#include "imgui.h"
#include "pch.h"

#include "ApplicationEvent.h"
#include "Core.h"
#include "EventManager.h"
#include "Singleton.h"
#include "Window.h"

namespace Doodle
{
class ApplicationRunner;

class DOO_API Application
{
public:
    class DOO_API Time
    {
    public:
        static float GetTime();
        static float GetDeltaTime();
        static float GetFPS();

    private:
        static float s_FPS;
        static float s_Time;
        static float s_DeltaTime;
        static float s_LastFrameTime;
        static void Update();
        static void Freeze();
        friend class Application;
    };

    friend class ApplicationRunner;

    static Application &Get();
    virtual void Initialize();
    virtual void Deinitialize();
    virtual void OnLayout();
    virtual void OnUpdate();

protected:
    void Run();
    bool OnWindowRefreshEvent(WindowRefreshEvent &e);
    bool OnWindowMoveEvent(WindowMoveEvent &e);
    bool OnWindowCloseEvent(WindowCloseEvent &e);
    bool OnAppLayoutEvent(AppLayoutEvent &e);
    std::weak_ptr<Window> m_window;
    bool m_running = true;
};

} // namespace Doodle