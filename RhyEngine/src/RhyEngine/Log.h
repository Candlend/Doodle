#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace RhyEngine
{

class RHY_API Log
{
public:
    static void Init();
    static void LoadConfig(const std::string &configFile);
    inline static std::shared_ptr<spdlog::logger> &GetCoreLogger();
    static std::shared_ptr<spdlog::logger> &GetClientLogger();

private:
    static void SetLogLevel(const std::string &levelStr, std::shared_ptr<spdlog::logger> &logger);
    static std::shared_ptr<spdlog::logger> s_coreLogger;
    static std::shared_ptr<spdlog::logger> s_clientLogger;
};

} // namespace RhyEngine

// Core log macros
#define RHY_CORE_TRACE(...) SPDLOG_LOGGER_CALL(::RhyEngine::Log::GetCoreLogger(), spdlog::level::trace, __VA_ARGS__)
#define RHY_CORE_DEBUG(...) SPDLOG_LOGGER_CALL(::RhyEngine::Log::GetCoreLogger(), spdlog::level::debug, __VA_ARGS__)
#define RHY_CORE_INFO(...) SPDLOG_LOGGER_CALL(::RhyEngine::Log::GetCoreLogger(), spdlog::level::info, __VA_ARGS__)
#define RHY_CORE_WARN(...) SPDLOG_LOGGER_CALL(::RhyEngine::Log::GetCoreLogger(), spdlog::level::warn, __VA_ARGS__)
#define RHY_CORE_ERROR(...) SPDLOG_LOGGER_CALL(::RhyEngine::Log::GetCoreLogger(), spdlog::level::err, __VA_ARGS__)

// Client log macros
#define RHY_TRACE(...) SPDLOG_LOGGER_CALL(::RhyEngine::Log::GetClientLogger(), spdlog::level::trace, __VA_ARGS__)
#define RHY_DEBUG(...) SPDLOG_LOGGER_CALL(::RhyEngine::Log::GetClientLogger(), spdlog::level::debug, __VA_ARGS__)
#define RHY_INFO(...) SPDLOG_LOGGER_CALL(::RhyEngine::Log::GetClientLogger(), spdlog::level::info, __VA_ARGS__)
#define RHY_WARN(...) SPDLOG_LOGGER_CALL(::RhyEngine::Log::GetClientLogger(), spdlog::level::warn, __VA_ARGS__)
#define RHY_ERROR(...) SPDLOG_LOGGER_CALL(::RhyEngine::Log::GetClientLogger(), spdlog::level::err, __VA_ARGS__)
