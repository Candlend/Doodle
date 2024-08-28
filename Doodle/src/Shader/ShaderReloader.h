#pragma once

#include "pch.h"
#include <filesystem>

#include "Shader.h"

namespace Doodle
{

class DOO_API ShaderReloader
{
public:
    ShaderReloader(const std::string &filepath, Shader &shader)
        : m_filepath(filepath), m_shader(shader), m_lastWriteTime(std::filesystem::last_write_time(filepath))
    {
        m_running = true;
        m_thread = std::thread(&ShaderReloader::Watch, this);
    }

    ~ShaderReloader()
    {
        m_running = false;
        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }

private:
    void Watch()
    {
        while (m_running)
        {
            auto currentWriteTime = std::filesystem::last_write_time(m_filepath);
            if (currentWriteTime != m_lastWriteTime)
            {
                m_lastWriteTime = currentWriteTime;
                m_shader.Reload();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 每100ms检查一次
        }
    }

    std::string m_filepath;
    Shader &m_shader;
    std::filesystem::file_time_type m_lastWriteTime;
    std::thread m_thread;
    bool m_running;
};

} // namespace Doodle
