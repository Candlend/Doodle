
#include "FileReloader.h"

namespace Doodle
{

FileReloader::FileReloader()
{
    m_running = true;
    m_thread = std::thread(&FileReloader::Watch, this);
}

FileReloader::~FileReloader()
{
    m_running = false;
    if (m_thread.joinable())
    {
        m_thread.join();
    }
    m_reloadables.clear();
}

void FileReloader::AddReloadable(IReloadable *reloadable)
{
    m_reloadables[reloadable->GetFilepath()] = reloadable;
}

void FileReloader::RemoveReloadable(IReloadable *reloadable)
{
    auto filepath = reloadable->GetFilepath();
    if (m_reloadables.find(filepath) == m_reloadables.end())
        m_reloadables.erase(reloadable->GetFilepath());
}

void FileReloader::Watch()
{
    while (m_running)
    {
        for (auto &reloadable : m_reloadables)
        {
            reloadable.second->TryReload();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

} // namespace Doodle