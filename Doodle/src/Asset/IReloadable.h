#pragma once

#include "pch.h"
#include <filesystem>

#include "ConfirmPopupModal.h"

namespace Doodle
{

class FileReloader;
class DOO_API IReloadable
{
public:
    IReloadable(const std::string &filepath = "");
    virtual ~IReloadable();
    void WatchFile(const std::string &filepath);
    virtual void TryReload() = 0;
    virtual void Reload() = 0;
    std::string GetFilepath() const
    {
        return m_filepath;
    }

protected:
    std::string m_filepath;
    std::filesystem::file_time_type m_lastWriteTime;
    std::shared_ptr<FileReloader> m_reloader;
};

class DOO_API IAutoReloadable : public IReloadable
{
public:
    IAutoReloadable(const std::string &filepath = "") : IReloadable(filepath)
    {
    }
    void TryReload() override;
};

class DOO_API IConfirmReloadable : public IReloadable
{
public:
    IConfirmReloadable(const std::string &filepath = "");
    void TryReload();

protected:
    void OnLayout();
    ConfirmPopupModal m_confirmPopupModal;
};

} // namespace Doodle