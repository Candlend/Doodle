#pragma once

#include "pch.h"

#include "Singleton.h"

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
    void CreateApp()
    {
        m_app = std::move(std::make_unique<T>());
    }

    void Run();

    Application &GetCurrentApp();

private:
    std::unique_ptr<Application> m_app;
};

} // namespace RhyEngine