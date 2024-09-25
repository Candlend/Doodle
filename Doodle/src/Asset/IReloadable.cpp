#include <imgui.h>

#include "ApplicationEvent.h"
#include "ApplicationRunner.h"
#include "EventManager.h"
#include "FileReloader.h"
#include "IReloadable.h"

namespace Doodle
{

IReloadable::IReloadable(const std::string &filepath, ReloadStrategy strategy)
    : m_filepath(filepath), m_strategy(strategy)
{
    m_reloader = FileReloader::Get();
    m_filepath = std::filesystem::absolute(filepath).string();
    if (m_filepath.empty())
        return;
    m_lastWriteTime = std::filesystem::last_write_time(filepath);
    m_reloader->AddReloadable(this);
}

IReloadable::~IReloadable()
{
    if (m_filepath.empty())
        return;
    m_reloader->RemoveReloadable(this);
}

void IReloadable::WatchFile(const std::string &filepath)
{
    m_filepath = filepath;
    m_lastWriteTime = std::filesystem::last_write_time(filepath);
    m_reloader->AddReloadable(this);
}

void IReloadable::TryReload()
{
    auto currentWriteTime = std::filesystem::last_write_time(m_filepath);
    if (currentWriteTime != m_lastWriteTime)
    {
        m_lastWriteTime = currentWriteTime;
        if (m_strategy == ReloadStrategy::Auto)
        {
            HandleAutoReload();
        }
        else if (m_strategy == ReloadStrategy::Confirm)
        {
            HandleConfirmReload();
        }
    }
}

void IReloadable::HandleAutoReload()
{
    Reload();
}

void IReloadable::HandleConfirmReload()
{
    m_confirmPopupModal.Bind(
        "Reload", [this]() { Reload(); },
        [this]() {
            // Handle cancel logic if needed
        },
        {
            [this]() {
                auto text = fmt::format(
                    "The file {} has been modified outside of the editor. Do you wish to reload it?", m_filepath);
                ImGui::TextUnformatted(text.c_str());
            },
        });

    if (!m_confirmPopupModal.IsOpened())
    {
        m_confirmPopupModal.Open();
    }
    m_confirmPopupModal.Draw();
}

} // namespace Doodle