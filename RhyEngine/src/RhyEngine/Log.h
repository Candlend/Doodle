#pragma once

#include <spdlog/spdlog.h>

#include <memory>

#include "Core.h"

namespace RhyEngine
{

class RHY_API Log
{
public:
    static void Init();

    inline static std::shared_ptr<spdlog::logger> &GetCoreLogger()
    {
        return CoreLogger;
    }
    inline static std::shared_ptr<spdlog::logger> &GetClientLogger()
    {
        return ClientLogger;
    }

private:
    static std::shared_ptr<spdlog::logger> CoreLogger;
    static std::shared_ptr<spdlog::logger> ClientLogger;
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