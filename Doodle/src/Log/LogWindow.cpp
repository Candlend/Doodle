#include "LogWindow.h"
#include "IconsFontAwesome6Pro.h"
#include "ImGuiUtils.h"
#include "Log.h"
#include "imgui.h"
#include <format>

namespace Doodle
{

void LogWindow::Initialize()
{
}

void LogWindow::OnUpdate()
{
    m_logCount = Log::GetLogCount(LogType::Log);
    m_warningCount = Log::GetLogCount(LogType::Warning);
    m_errorCount = Log::GetLogCount(LogType::Error);
}

void LogWindow::OnLayout()
{
    ImGuiUtils::WindowScope ws("Log");
    if (ImGui::Button("清空"))
    {
        Log::Clear();
    }

    ImGui::SameLine();
    ImGui::InputText(ICON_FA_MAGNIFYING_GLASS, m_filter, sizeof(m_filter));
    ImGui::SameLine();

    ImColor grayColor = ImGuiUtils::GetColor(0x808080);
    {
        ImGuiUtils::StyleColorScope sc(ImGuiCol_Text,
                                       m_logCount > 0 ? ImGuiUtils::GetColor(m_colorMap[LogType::Log]) : grayColor);
        std::string label =
            std::format("{} {}", ICON_FA_CIRCLE_INFO, m_logCount > 99 ? "99+" : std::to_string(m_logCount));
        ImGuiUtils::ToggleButton(label.c_str(), m_showLog);
    }
    ImGui::SameLine(0, 2);
    {
        ImGuiUtils::StyleColorScope sc(
            ImGuiCol_Text, m_warningCount > 0 ? ImGuiUtils::GetColor(m_colorMap[LogType::Warning]) : grayColor);
        std::string label = std::format("{} {}", ICON_FA_TRIANGLE_EXCLAMATION,
                                        m_warningCount > 99 ? "99+" : std::to_string(m_warningCount));
        ImGuiUtils::ToggleButton(label.c_str(), m_showWarning);
    }
    ImGui::SameLine(0, 2);
    {
        ImGuiUtils::StyleColorScope sc(ImGuiCol_Text,
                                       m_errorCount > 0 ? ImGuiUtils::GetColor(m_colorMap[LogType::Error]) : grayColor);
        std::string label = std::format("{} {}", ICON_FA_BUG, m_errorCount > 99 ? "99+" : std::to_string(m_errorCount));
        ImGuiUtils::ToggleButton(label.c_str(), m_showError);
    }

    ImGui::Separator();
    {
        ImGuiUtils::ChildWindowScope cws("scrolling", ImVec2(0, 0), ImGuiWindowFlags_HorizontalScrollbar);
        if (cws.IsOpened())
        {
            for (const LogInfo &logInfo : Log::GetLogs())
            {
                std::string filter(m_filter);
                if (!filter.empty() && logInfo.Message.find(filter) == std::string::npos)
                    continue;
                if (logInfo.Type == LogType::Log && !m_showLog)
                    continue;
                if (logInfo.Type == LogType::Warning && !m_showWarning)
                    continue;
                if ((logInfo.Type == LogType::Error) && !m_showError)
                    continue;
                ImGuiUtils::StyleColorScope sc(ImGuiCol_Text, ImGuiUtils::GetColor(m_colorMap[logInfo.Type]));
                if (ImGui::Selectable(logInfo.Message.c_str(), false))
                {
                    ImGui::SetClipboardText((logInfo.Message + "\n" + logInfo.Stacktrace).c_str());
                }
                if (ImGui::IsItemHovered() && !logInfo.Stacktrace.empty())
                {
                    ImGui::SetTooltip("%s", logInfo.Stacktrace.c_str());
                }
            }

            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            {
                ImGui::SetScrollHereY(1.0f);
            }
        }
    }
}

void LogWindow::Deinitialize()
{
}

} // namespace Doodle
