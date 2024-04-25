#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Doodle
{

class DOO_API Log
{
public:
    static void Init();
    static void LoadConfig(const std::string &configFile);
    static std::shared_ptr<spdlog::logger> &GetCoreLogger();
    static std::shared_ptr<spdlog::logger> &GetClientLogger();

private:
    static void SetLogLevel(const std::string &levelStr, std::shared_ptr<spdlog::logger> &logger);
    static std::shared_ptr<spdlog::logger> s_CoreLogger;
    static std::shared_ptr<spdlog::logger> s_ClientLogger;
};

} // namespace Doodle

// Core log macros
#define DOO_CORE_TRACE(...) SPDLOG_LOGGER_CALL(::Doodle::Log::GetCoreLogger(), spdlog::level::trace, __VA_ARGS__)
#define DOO_CORE_DEBUG(...) SPDLOG_LOGGER_CALL(::Doodle::Log::GetCoreLogger(), spdlog::level::debug, __VA_ARGS__)
#define DOO_CORE_INFO(...) SPDLOG_LOGGER_CALL(::Doodle::Log::GetCoreLogger(), spdlog::level::info, __VA_ARGS__)
#define DOO_CORE_WARN(...) SPDLOG_LOGGER_CALL(::Doodle::Log::GetCoreLogger(), spdlog::level::warn, __VA_ARGS__)
#define DOO_CORE_ERROR(...) SPDLOG_LOGGER_CALL(::Doodle::Log::GetCoreLogger(), spdlog::level::err, __VA_ARGS__)

// Client log macros
#define DOO_TRACE(...) SPDLOG_LOGGER_CALL(::Doodle::Log::GetClientLogger(), spdlog::level::trace, __VA_ARGS__)
#define DOO_DEBUG(...) SPDLOG_LOGGER_CALL(::Doodle::Log::GetClientLogger(), spdlog::level::debug, __VA_ARGS__)
#define DOO_INFO(...) SPDLOG_LOGGER_CALL(::Doodle::Log::GetClientLogger(), spdlog::level::info, __VA_ARGS__)
#define DOO_WARN(...) SPDLOG_LOGGER_CALL(::Doodle::Log::GetClientLogger(), spdlog::level::warn, __VA_ARGS__)
#define DOO_ERROR(...) SPDLOG_LOGGER_CALL(::Doodle::Log::GetClientLogger(), spdlog::level::err, __VA_ARGS__)
