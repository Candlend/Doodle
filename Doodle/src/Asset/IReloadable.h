#pragma once

#include "pch.h"
#include <filesystem>

namespace Doodle
{

class FileReloader;
class IReloadable
{
public:
    IReloadable(const std::string &filepath = "");
    virtual ~IReloadable();
    void TryReload();
    virtual void Reload() = 0;
    std::string GetFilepath() const
    {
        return m_filepath;
    }

private:
    std::string m_filepath;
    std::filesystem::file_time_type m_lastWriteTime;
    std::shared_ptr<FileReloader> m_reloader;
};

} // namespace Doodle