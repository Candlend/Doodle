#pragma once
#include "pch.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include "Core.h"

namespace RhyEngine
{

static std::shared_ptr<spdlog::logger> g_CoreLogger;    

class RHY_API Log
{
public:
    static void Init();

    inline static std::shared_ptr<spdlog::logger> &GetCoreLogger()
    {
        std::cout << "11" << std::endl;
        return s_coreLogger;
    }
    inline static std::shared_ptr<spdlog::logger> &GetClientLogger()
    {
        return s_clientLogger;
    }

    // 删除构造函数，防止实例化
    Log() = delete;
    Log(const Log &) = delete;
    Log &operator=(const Log &) = delete;

private:
    static std::shared_ptr<spdlog::logger> s_coreLogger;
    static std::shared_ptr<spdlog::logger> s_clientLogger;
};

} // namespace RhyEngine

// Core log macros
#define RHY_CORE_TRACE(...) ::RhyEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define RHY_CORE_INFO(...) ::RhyEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define RHY_CORE_WARN(...) ::RhyEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define RHY_CORE_ERROR(...) ::RhyEngine::Log::GetCoreLogger()->error(__VA_ARGS__)

// Client log macros
#define RHY_TRACE(...) ::RhyEngine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define RHY_INFO(...) ::RhyEngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define RHY_WARN(...) ::RhyEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define RHY_ERROR(...) ::RhyEngine::Log::GetClientLogger()->error(__VA_ARGS__)