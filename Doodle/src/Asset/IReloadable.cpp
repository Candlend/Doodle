
#include "IReloadable.h"
#include "FileReloader.h"

namespace Doodle
{

IReloadable::IReloadable(const std::string &filepath) : m_filepath(filepath)
{
    m_filepath = std::filesystem::absolute(filepath).string();
    if (m_filepath.empty())
        return;
    m_lastWriteTime = std::filesystem::last_write_time(filepath);
    m_reloader = FileReloader::Get();
    m_reloader->AddReloadable(this);
}

IReloadable::~IReloadable()
{
    if (m_filepath.empty())
        return;
    m_reloader->RemoveReloadable(this);
}

void IReloadable::TryReload()
{
    auto currentWriteTime = std::filesystem::last_write_time(m_filepath);
    if (currentWriteTime != m_lastWriteTime)
    {
        m_lastWriteTime = currentWriteTime;
        Reload();
    }
}

} // namespace Doodle