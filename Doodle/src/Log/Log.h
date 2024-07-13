#pragma once

#include "Core.h"
#include "spdlog/common.h"
#include <chrono>
#include <cstddef>
#include <queue>
#include <spdlog/spdlog.h>
#include <unordered_map>

#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/base_sink.h>

namespace Doodle
{

enum class LogType
{
    Trace,
    Debug,
    Info,
    Warning,
    Error,
};

using time_point = std::chrono::time_point<std::chrono::system_clock>;

// 日志结构
struct LogInfo
{
    time_point Time;
    std::string Message;
    std::string Stacktrace;
    size_t Hash;
    LogType Type;
};

struct CollapsedLogInfo : LogInfo
{
    int Count;
};

class DOO_API Log
{
public:
    static void Initialize();
    static void LoadConfig(const std::string &configFile);
    static std::shared_ptr<spdlog::logger> &GetCoreLogger();
    static std::shared_ptr<spdlog::logger> &GetClientLogger();
    static const std::deque<LogInfo> &GetLogInfos();
    static const std::unordered_map<size_t, CollapsedLogInfo> &GetCollapsedLogInfos();
    static int GetLogCount(LogType type);
    static void Clear();

private:
    class ImGuiLogSink;
    static void SetLogLevel(const std::string &levelStr, std::shared_ptr<spdlog::logger> &logger);
    static std::shared_ptr<spdlog::logger> s_CoreLogger;
    static std::shared_ptr<spdlog::logger> s_ClientLogger;
    static std::deque<LogInfo> s_LogInfos;
    static std::unordered_map<LogType, int> s_LogInfoCount;
    static std::unordered_map<size_t, CollapsedLogInfo> s_CollapsedLogInfos;
    static const int MAX_LOG_COUNT = 1000;
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
