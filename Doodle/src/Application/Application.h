#pragma once

#include "pch.h"
#include <imgui.h>

#include "ApplicationEvent.h"
#include "Core.h"
#include "EventManager.h"
#include "ImGuiBuilder.h"
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

    Application();
    virtual ~Application();
    virtual void Initialize();
    virtual void Deinitialize();
    virtual void BeforeUpdate() {};
    virtual void AfterUpdate() {};
    virtual void BeforeLayout() {};
    virtual void AfterLayout() {};
    virtual void BeforeRender() {};
    virtual void AfterRender() {};

protected:
    void Run();
    bool OnWindowRefresh(WindowRefreshEvent &e);
    bool OnWindowMove(WindowMoveEvent &e);
    bool OnWindowClose(WindowCloseEvent &e);
    bool m_running = true;
};

} // namespace Doodle