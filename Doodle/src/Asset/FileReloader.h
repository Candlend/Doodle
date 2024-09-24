#pragma once

#include "pch.h"

#include "IReloadable.h"
#include "Singleton.h"

namespace Doodle
{
class FileReloader : public Singleton<FileReloader>
{
public:
    FileReloader();

    ~FileReloader();

    void AddReloadable(IReloadable *reloadable);
    void RemoveReloadable(IReloadable *reloadable);

private:
    void Watch();
    std::unordered_map<std::string, IReloadable *> m_reloadables;
    std::string m_filepath;
    std::function<void()> m_callback;
    std::thread m_thread;
    bool m_running;
};

} // namespace Doodle