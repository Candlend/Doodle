#pragma once

#include "pch.h"

#include "ConfirmPopupModal.h"
#include <filesystem>

namespace Doodle
{

enum class ReloadStrategy
{
    Auto,
    Confirm
};

class FileReloader;

class DOO_API IReloadable
{
public:
    IReloadable(const std::string &filepath = "", ReloadStrategy strategy = ReloadStrategy::Auto);
    virtual ~IReloadable();
    void WatchFile(const std::string &filepath);
    void TryReload();
    virtual void Reload() = 0;
    std::string GetFilepath() const
    {
        return m_filepath;
    }

    void SetStrategy(ReloadStrategy strategy)
    {
        m_strategy = strategy;
    }

    ReloadStrategy GetStrategy() const
    {
        return m_strategy;
    }

protected:
    std::string m_filepath;
    std::filesystem::file_time_type m_lastWriteTime;
    std::shared_ptr<FileReloader> m_reloader;
    ReloadStrategy m_strategy;
    ConfirmPopupModal m_confirmPopupModal;

    void HandleAutoReload();
    void HandleConfirmReload();
};

} // namespace Doodle