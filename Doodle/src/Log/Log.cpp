#include "pch.h"
#include <boost/stacktrace.hpp>
#include <chrono>
#include <cstddef>
#include <nlohmann/json.hpp>
#include <queue>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <string>
#include <unordered_map>

#include "Log.h"
#include "Utils.h"
#include "spdlog/fmt/bundled/format.h"
#include "spdlog/logger.h"

using json = nlohmann::json;

namespace Doodle
{

std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
std::deque<LogInfo> Log::s_LogInfos;
std::unordered_map<LogType, int> Log::s_LogInfoCount;
std::unordered_map<size_t, CollapsedLogInfo> Log::s_CollapsedLogInfos;

int Log::GetLogCount(LogType type)
{
    return Log::s_LogInfoCount[type];
}

class Log::ImGuiLogSink : public spdlog::sinks::base_sink<std::mutex>
{
public:
    void sink_it_(const spdlog::details::log_msg &msg) override
    {
        spdlog::source_loc loc = msg.source;
        LogType logType = LogType::Info;
        switch (msg.level)
        {
        case spdlog::level::trace:
            logType = LogType::Trace;
            break;
        case spdlog::level::debug:
            logType = LogType::Debug;
            break;
        case spdlog::level::info:
            logType = LogType::Info;
            break;
        case spdlog::level::warn:
            logType = LogType::Warning;
            break;
        case spdlog::level::err:
            logType = LogType::Error;
            break;
        case spdlog::level::critical:
            logType = LogType::Error;
        case spdlog::level::off:
            break;
        case spdlog::level::n_levels:
            break;
        }
        Log::s_LogInfoCount[logType]++;
        std::string message = fmt::to_string(msg.payload);
        time_point msgTime = msg.time;
        // 将日志消息添加到日志列表
        boost::stacktrace::stacktrace st = boost::stacktrace::stacktrace();
        // 将 stacktrace 的条目存储到 vector 中
        std::vector<boost::stacktrace::frame> frames(st.begin(), st.end());
        std::ostringstream oss;

        for (size_t i = 5; i < frames.size(); i++)
        {
            std::string sourceFile = frames[i].source_file();
            std::string functionName = frames[i].name();
            std::string line = std::to_string(frames[i].source_line());
            if (sourceFile.find("Doodle") == std::string::npos)
            {
                continue;
            }
            std::string frameStr = fmt::format("({}:{}) {}", sourceFile, line, functionName);
            oss << frameStr << std::endl;
        }
        std::string stStr = oss.str();
        std::hash<std::string> hashFunc;
        size_t hashValue = hashFunc(message);
        Log::s_LogInfos.push_back(LogInfo{msgTime, message, stStr, hashValue, logType});
        if (Log::s_CollapsedLogInfos.contains(hashValue))
        {
            Log::s_CollapsedLogInfos[hashValue].Time = msgTime;
            Log::s_CollapsedLogInfos[hashValue].Message = message;
            Log::s_CollapsedLogInfos[hashValue].Count++;
        }
        else
        {
            Log::s_CollapsedLogInfos[hashValue] = CollapsedLogInfo{Log::s_LogInfos.back(), 1};
        }
        // 限制日志数量
        while (Log::s_LogInfos.size() > Log::MAX_LOG_COUNT)
        {
            Log::s_LogInfos.pop_front();
        }
    }

    void flush_() override
    {
        // 此处可以实现 flush 逻辑（如写入文件），但在 ImGui 中通常不需要
    }
};

void Log::Clear()
{
    Log::s_LogInfos.clear();
    Log::s_CollapsedLogInfos.clear();
    for (auto &[type, count] : Log::s_LogInfoCount)
    {
        count = 0;
    }
}

const std::deque<LogInfo> &Log::GetLogInfos()
{
    return Log::s_LogInfos;
}

const std::unordered_map<size_t, CollapsedLogInfo> &Log::GetCollapsedLogInfos()
{
    return Log::s_CollapsedLogInfos;
}

void Log::Initialize()
{
    try
    {
        s_CoreLogger = std::make_shared<spdlog::logger>("CORE");
        s_ClientLogger = std::make_shared<spdlog::logger>("CLIENT");
        Log::LoadConfig("config/log.json");
    }
    catch (const std::exception &e)
    {
        // 处理初始化失败的情况
        std::cerr << "Log initialization failed: " << e.what() << std::endl;
        // 可以选择退出或使用默认的日志处理
    }
}

void Log::LoadConfig(const std::string &configFile)
{
    std::string normalizedConfigFile = NormalizePath(configFile);
    std::ifstream file(configFile);
    if (!file.is_open())
    {
        std::cerr << "Could not open log config file: " << configFile << std::endl;
        return;
    }

    nlohmann::json config;
    try
    {
        file >> config;
    }
    catch (const nlohmann::json::parse_error &e)
    {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return;
    }

    // 读取核心日志配置
    if (config.contains("core"))
    {
        auto coreConfig = config["core"];
        std::string coreLogLevel = coreConfig.value("log_level", "info");

        // 读取日志文件输出设置
        std::string coreLogFile = coreConfig.value("log_file", "logs/core.log");
        std::string coreLogFileSizeStr = coreConfig.value("log_file_size", "5MB");
        int coreLogFileCount = coreConfig.value("log_file_count", 3);

        // 将文件大小字符串转换为字节数
        size_t coreLogFileSize = 5 * 1024 * 1024; // 默认 5MB
        if (coreLogFileSizeStr.ends_with("MB"))
        {
            coreLogFileSize = std::stoi(coreLogFileSizeStr.substr(0, coreLogFileSizeStr.size() - 2)) * 1024 * 1024;
        }
        else if (coreLogFileSizeStr.ends_with("KB"))
        {
            coreLogFileSize = std::stoi(coreLogFileSizeStr.substr(0, coreLogFileSizeStr.size() - 2)) * 1024;
        }

        auto coreConsoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        s_CoreLogger->sinks().push_back(coreConsoleSink);
        // 创建核心旋转文件输出日志器
        auto coreRotatingSink =
            std::make_shared<spdlog::sinks::rotating_file_sink_mt>(coreLogFile, coreLogFileSize, coreLogFileCount);
        s_CoreLogger->sinks().push_back(coreRotatingSink);

        SetLogLevel(coreLogLevel, s_CoreLogger);

        // 创建 ImGui 日志输出
        auto coreImGuiSink = std::make_shared<ImGuiLogSink>();
        coreImGuiSink->set_level(spdlog::level::trace);
        s_CoreLogger->sinks().push_back(coreImGuiSink);

        // 读取日志格式
        std::string coreLogPattern = coreConfig.value("log_pattern", "%^[%Y-%m-%d %H:%M:%S] %n: %v%$");
        s_CoreLogger->set_pattern(coreLogPattern);
        s_CoreLogger->set_level(spdlog::level::trace);
    }

    // 读取客户端日志配置
    if (config.contains("client"))
    {
        auto clientConfig = config["client"];
        std::string clientLogLevel = clientConfig.value("log_level", "info");

        // 读取日志文件输出设置
        std::string clientLogFile = clientConfig.value("log_file", "logs/client.log");
        std::string clientLogFileSizeStr = clientConfig.value("log_file_size", "5MB");
        int clientLogFileCount = clientConfig.value("log_file_count", 3);

        // 将文件大小字符串转换为字节数
        size_t clientLogFileSize = 5 * 1024 * 1024; // 默认 5MB
        if (clientLogFileSizeStr.ends_with("MB"))
        {
            clientLogFileSize =
                std::stoi(clientLogFileSizeStr.substr(0, clientLogFileSizeStr.size() - 2)) * 1024 * 1024;
        }
        else if (clientLogFileSizeStr.ends_with("KB"))
        {
            clientLogFileSize = std::stoi(clientLogFileSizeStr.substr(0, clientLogFileSizeStr.size() - 2)) * 1024;
        }

        auto clientConsoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        s_ClientLogger->sinks().push_back(clientConsoleSink);
        // 创建客户端旋转文件输出日志器
        auto clientRotatingSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            clientLogFile, clientLogFileSize, clientLogFileCount);
        s_ClientLogger->sinks().push_back(clientRotatingSink);

        SetLogLevel(clientLogLevel, s_ClientLogger);

        // 创建 ImGui 日志输出
        auto clientImGuiSink = std::make_shared<ImGuiLogSink>();
        clientImGuiSink->set_level(spdlog::level::trace);
        s_ClientLogger->sinks().push_back(clientImGuiSink);

        // 读取日志格式
        std::string clientLogPattern = clientConfig.value("log_pattern", "%^[%Y-%m-%d %H:%M:%S] %n: %v%$");
        s_ClientLogger->set_pattern(clientLogPattern);
        s_ClientLogger->set_level(spdlog::level::trace);
    }
}

void Log::SetLogLevel(const std::string &levelStr, std::shared_ptr<spdlog::logger> &logger)
{
    for (auto &sink : logger->sinks())
    {
        if (levelStr == "trace")
            sink->set_level(spdlog::level::trace);
        else if (levelStr == "debug")
            sink->set_level(spdlog::level::debug);
        else if (levelStr == "info")
            sink->set_level(spdlog::level::info);
        else if (levelStr == "warn")
            sink->set_level(spdlog::level::warn);
        else if (levelStr == "error")
            sink->set_level(spdlog::level::err);
        else if (levelStr == "critical")
            sink->set_level(spdlog::level::critical);
        else
        {
            std::cerr << "Unknown log level: " << levelStr << ", using default 'info'" << std::endl;
            sink->set_level(spdlog::level::info);
        }
    }
}

std::shared_ptr<spdlog::logger> &Log::GetCoreLogger()
{
    return s_CoreLogger;
}

std::shared_ptr<spdlog::logger> &Log::GetClientLogger()
{
    return s_ClientLogger;
}

} // namespace Doodle
