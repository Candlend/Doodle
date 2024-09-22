#include "Utils.h"
#include "Log.h"

std::string NormalizePath(const std::string &path)
{
    std::string normalized = path;
    for (char &c : normalized)
    {
#ifdef DOO_PLATFORM_WINDOWS
        if (c == '/')
        {
            c = '\\';
        }
#else
        if (c == '\\')
        {
            c = '/';
        }
#endif
    }

    return normalized;
}

std::string FormatTimePoint(const std::chrono::time_point<std::chrono::system_clock> &tp)
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

DOO_API void PrintBinary(const void *ptr, size_t size)
{
    uint8_t *bytePtr = reinterpret_cast<uint8_t *>(const_cast<void *>(ptr)); // 将 void * 转换为 uint8_t *
    std::ostringstream oss;
    for (size_t i = 0; i < size; i++)
    {
        for (int j = 7; j >= 0; j--)
        { // 从高位到低位输出每个字节
            oss << ((bytePtr[i] & (1 << j)) ? '1' : '0');
        }
        oss << ' '; // 每个字节之间加个空格
    }
    DOO_CORE_DEBUG("Binary: {0}", oss.str());
}

DOO_API std::string RemoveExtension(const std::string &filename)
{
    size_t lastindex = filename.find_last_of('.');
    return filename.substr(0, lastindex);
}

DOO_API std::string GetDirectory(const std::string &path)
{
    size_t lastindex = path.find_last_of("/\\");
    return path.substr(0, lastindex);
}
