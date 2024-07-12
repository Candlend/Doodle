#pragma once

#include "pch.h"
#include <chrono>

inline std::string NormalizePath(const std::string &path)
{
    std::string normalized = path;
    for (char &c : normalized) {
#ifdef DOO_PLATFORM_WINDOWS
        if (c == '/') {
            c = '\\';
        }
#else
        if (c == '\\') {
            c = '/';
        }
#endif
    }

    return normalized;
}

inline std::string FormatTimePoint(const std::chrono::time_point<std::chrono::system_clock> &tp)
{
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    // 将 time_t 转换为 tm 结构
    tm ltm;
    localtime_s(&ltm, &t); // Windows
    // localtime_r(&time_t_value, &ltm); // POSIX

    // 使用 fmt 库格式化时间
    return fmt::format("{:04}-{:02}-{:02} {:02}:{:02}:{:02}", ltm.tm_year + 1900, ltm.tm_mon + 1, ltm.tm_mday,
                       ltm.tm_hour, ltm.tm_min, ltm.tm_sec);
}
