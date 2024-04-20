#include "pch.h"
#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include <nlohmann/json.hpp>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Utils.h"
#include "Log.h"

using json = nlohmann::json;

namespace Doodle
{

std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

void Log::Init()
{
    try
    {
        s_CoreLogger = spdlog::stdout_color_mt("CORE");
        s_ClientLogger = spdlog::stdout_color_mt("CLIENT");
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
        SetLogLevel(coreLogLevel, s_CoreLogger);

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

        // 创建核心旋转文件输出日志器
        auto coreRotatingSink =
            std::make_shared<spdlog::sinks::rotating_file_sink_mt>(coreLogFile, coreLogFileSize, coreLogFileCount);
        s_CoreLogger->sinks().push_back(coreRotatingSink);

        // 读取日志格式
        std::string coreLogPattern = coreConfig.value("log_pattern", "%^[%Y-%m-%d %H:%M:%S] %n: %v%$");
        s_CoreLogger->set_pattern(coreLogPattern);
    }

    // 读取客户端日志配置
    if (config.contains("client"))
    {
        auto clientConfig = config["client"];
        std::string clientLogLevel = clientConfig.value("log_level", "info");
        SetLogLevel(clientLogLevel, s_ClientLogger);

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

        // 创建客户端旋转文件输出日志器
        auto clientRotatingSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            clientLogFile, clientLogFileSize, clientLogFileCount);
        s_ClientLogger->sinks().push_back(clientRotatingSink);

        // 读取日志格式
        std::string clientLogPattern = clientConfig.value("log_pattern", "%^[%Y-%m-%d %H:%M:%S] %n: %v%$");
        s_ClientLogger->set_pattern(clientLogPattern);
    }
}

void Log::SetLogLevel(const std::string &levelStr, std::shared_ptr<spdlog::logger> &logger)
{
    if (levelStr == "trace")
        logger->set_level(spdlog::level::trace);
    else if (levelStr == "debug")
        logger->set_level(spdlog::level::debug);
    else if (levelStr == "info")
        logger->set_level(spdlog::level::info);
    else if (levelStr == "warn")
        logger->set_level(spdlog::level::warn);
    else if (levelStr == "error")
        logger->set_level(spdlog::level::err);
    else if (levelStr == "critical")
        logger->set_level(spdlog::level::critical);
    else
    {
        std::cerr << "Unknown log level: " << levelStr << ", using default 'info'" << std::endl;
        logger->set_level(spdlog::level::info);
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
