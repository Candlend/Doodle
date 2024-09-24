#include <imgui.h>

#include "ApplicationEvent.h"
#include "ApplicationRunner.h"
#include "EventManager.h"
#include "FileReloader.h"
#include "IReloadable.h"

namespace Doodle
{

IReloadable::IReloadable(const std::string &filepath) : m_filepath(filepath)
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

void IAutoReloadable::TryReload()
{
    auto currentWriteTime = std::filesystem::last_write_time(m_filepath);
    if (currentWriteTime != m_lastWriteTime)
    {
        m_lastWriteTime = currentWriteTime;
        Reload();
    }
}

IConfirmReloadable::IConfirmReloadable(const std::string &filepath) : IReloadable(filepath)
{
    m_confirmPopupModal.Bind(
        "Reload",
        [this]() {
            m_lastWriteTime = std::filesystem::last_write_time(m_filepath);
            EventManager::Get()->RemoveListener<AppLayoutEvent>(this, &IConfirmReloadable::OnLayout);
            Reload();
        },
        [this]() {
            m_lastWriteTime = std::filesystem::last_write_time(m_filepath);
            EventManager::Get()->RemoveListener<AppLayoutEvent>(this, &IConfirmReloadable::OnLayout);
        },
        {
            [this]() {
                auto text = fmt::format(
                    "The file {} has been modified outside of the editor. Do you wish to reload it?", m_filepath);
                ImGui::TextUnformatted(text.c_str());
            },
        });
}

void IConfirmReloadable::TryReload()
{
    auto currentWriteTime = std::filesystem::last_write_time(m_filepath);
    if (currentWriteTime != m_lastWriteTime)
    {
        EventManager::Get()->AddListener<AppLayoutEvent>(this, &IConfirmReloadable::OnLayout);
    }
}

void IConfirmReloadable::OnLayout()
{
    if (!m_confirmPopupModal.IsOpened())
    {
        m_confirmPopupModal.Open();
    }
    m_confirmPopupModal.Draw();
}

} // namespace Doodle