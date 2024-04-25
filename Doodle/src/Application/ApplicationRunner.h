#pragma once

#include "Core.h"
#include "pch.h"

#include "Singleton.h"
#include "Window.h"
#include "Application.h"
#include "SplashScreen.h"


namespace Doodle
{

class Application;

class DOO_API ApplicationRunner : public Singleton<ApplicationRunner>
{
public:
    ApplicationRunner() {
        DOO_CORE_TRACE("ApplicationRunner Start");
    }
    ~ApplicationRunner() {
        DOO_CORE_TRACE("ApplicationRunner End");
    }

    template <typename T>
    void CreateApp(WindowProps props = WindowProps()) // 目前仅支持一个应用程序一个窗口
    {
#ifndef DOO_HIDE_SPLASH
        m_window = Window::Create(props, false);
        if (!m_splashScreen.LoadImage("assets/splash.png")) {
            DOO_CORE_ERROR("Failed to load splash screen image!");
        }
        m_splashScreen.Begin(2.0f);
#else
        m_window = Window::Create(props, true);
#endif
        m_app = std::make_shared<T>();
        m_app->m_window = m_window;
        m_app->Initialize();

        m_splashScreen.End();
    }

    void Run();

    static Window &GetCurrentWindow(){
        DOO_CORE_ASSERT(Get().m_window, "No window created!");
        return *Get().m_window;
    }

    static Application &GetCurrentApp(){
        DOO_CORE_ASSERT(Get().m_app, "No application created!");
        return *Get().m_app;
    }

private:
    std::shared_ptr<Window> m_window;
    std::shared_ptr<Application> m_app;
    SplashScreen m_splashScreen;
};

} // namespace Doodle