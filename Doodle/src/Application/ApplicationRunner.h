#pragma once

#include "Log.h"
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
    ~ApplicationRunner()
    {
    }

    template <typename T>
    void CreateApp(WindowProps props = WindowProps()) // 目前仅支持一个应用程序一个窗口
    {
        std::string iconSmall = "assets/icons/icon_small.png";
        std::string iconLarge = "assets/icons/icon_large.png";
#ifndef DOO_HIDE_SPLASH
        m_window = Window::Create(props, false);
        if (!m_splashScreen.Load("assets/splash.png"))
        {
            DOO_CORE_ERROR("Failed to load splash screen image!");
        }
        // 设置m_splashScreen的图标和Window的图标一致
        m_splashScreen.Begin(2.0f);
        SetIcon(m_splashScreen.GetNativeWindow(), iconSmall, iconLarge);
#else
        m_window = Window::Create(props, true);
#endif
        SetIcon(m_window->GetNativeWindow(), iconSmall, iconLarge);
        m_app = T::Create();
        m_app->Initialize();
        m_splashScreen.End();
    }

#ifdef DOO_BUILD_PYTHON
    void CreateApp(py::object appClass, WindowProps props = WindowProps())
    {
        std::string iconSmall = "assets/icons/icon_small.png";
        std::string iconLarge = "assets/icons/icon_large.png";
#ifndef DOO_HIDE_SPLASH
        m_window = Window::Create(props, false);
        if (!m_splashScreen.Load("assets/splash.png"))
        {
            DOO_CORE_ERROR("Failed to load splash screen image!");
        }
        // 设置m_splashScreen的图标和Window的图标一致
        m_splashScreen.Begin(2.0f);
        SetIcon(m_splashScreen.GetNativeWindow(), iconSmall, iconLarge);
#else
        m_window = Window::Create(props, true);
#endif
        SetIcon(m_window->GetNativeWindow(), iconSmall, iconLarge);
        py::object appInstance = appClass.attr("create")();
        m_app = appInstance.cast<std::shared_ptr<Application>>();
        m_app->Initialize();
        m_splashScreen.End();
    }
#endif

    void Run();

    static std::shared_ptr<Window> GetWindow()
    {
        DOO_CORE_ASSERT(Get()->m_window, "No window created!");
        return Get()->m_window;
    }

    static std::shared_ptr<Application> GetApplication()
    {
        DOO_CORE_ASSERT(Get()->m_app, "No application created!");
        return Get()->m_app;
    }

    static void SetIcon(void *window, const std::string &iconSmall, const std::string &iconLarge);

private:
    std::shared_ptr<Window> m_window;
    std::shared_ptr<Application> m_app;
    SplashScreen m_splashScreen;
};

} // namespace Doodle