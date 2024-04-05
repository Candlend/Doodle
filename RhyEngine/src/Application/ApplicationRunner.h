#pragma once

#include "Core.h"
#include "pch.h"

#include "Singleton.h"
#include "Window.h"
#include "Application.h"
#include <memory>

namespace RhyEngine
{

class Application;

class RHY_API ApplicationRunner : public Singleton<ApplicationRunner>
{
public:
    ApplicationRunner() {
        RHY_CORE_TRACE("ApplicationRunner Start");
    }
    ~ApplicationRunner() {
        RHY_CORE_TRACE("ApplicationRunner End");
    }

    template <typename T>
    void CreateApp(WindowProps props = WindowProps()) // 目前仅支持一个应用程序一个窗口
    {
        m_window = Window::Create(props);
        m_app = std::make_shared<T>();
        m_app->m_window = m_window;
        m_app->Initialize();
    }

    void Run();

    static Window &GetCurrentWindow(){
        RHY_CORE_ASSERT(Get().m_window, "No window created!");
        return *Get().m_window;
    }

    static Application &GetCurrentApp(){
        RHY_CORE_ASSERT(Get().m_app, "No application created!");
        return *Get().m_app;
    }

private:
    std::shared_ptr<Window> m_window;
    std::shared_ptr<Application> m_app;
};

} // namespace RhyEngine