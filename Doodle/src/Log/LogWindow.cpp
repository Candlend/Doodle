#include "LogWindow.h"
#include "IconsFontAwesome6Pro.h"
#include "ImGuiUtils.h"
#include "Log.h"
#include "Utils.h"
#include "imgui.h"
#include <string>

namespace Doodle
{

void LogWindow::Initialize()
{
}

void LogWindow::OnUpdate()
{
    m_logCount = Log::GetLogCount(LogType::Trace) + Log::GetLogCount(LogType::Debug) + Log::GetLogCount(LogType::Info);
    m_warningCount = Log::GetLogCount(LogType::Warning);
    m_errorCount = Log::GetLogCount(LogType::Error);
}

bool LogWindow::ShouldSkip(const LogInfo &logInfo)
{
    std::string filter(m_filter);
    if (!filter.empty() && logInfo.Message.find(filter) == std::string::npos)
        return true;
    if (logInfo.Type == LogType::Trace && !m_showLog)
        return true;
    if (logInfo.Type == LogType::Debug && !m_showLog)
        return true;
    if (logInfo.Type == LogType::Info && !m_showLog)
        return true;
    if (logInfo.Type == LogType::Warning && !m_showWarning)
        return true;
    if ((logInfo.Type == LogType::Error) && !m_showError)
        return true;
    return false;
}

void LogWindow::OnLayout()
{
    std::string title = "日志";
    ImGuiUtils::WindowScope ws(title.c_str());

    if (ImGui::Button("清空"))
    {
        Log::Clear();
    }

    ImGui::SameLine();
    ImGui::InputText(ICON_FA_MAGNIFYING_GLASS, m_filter, sizeof(m_filter));
    ImGui::SameLine();

    ImGui::Checkbox("折叠", &m_collapse);

    ImGui::SameLine();

    ImColor grayColor = ImGuiUtils::GetColor(0x808080);
    {
        ImGuiUtils::StyleColorScope sc(ImGuiCol_Text,
                                       m_logCount > 0 ? ImGuiUtils::GetColor(m_colorMap[LogType::Trace]) : grayColor);
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

            if (!m_collapse)
            {
                for (const LogInfo &logInfo : Log::GetLogInfos())
                {
                    if (ShouldSkip(logInfo))
                        continue;
                    std::string label = FormatTimePoint(logInfo.Time) + " -";
                    if (ImGui::Selectable(label.c_str(), false))
                    {
                        ImGui::SetClipboardText((logInfo.Message + "\n" + logInfo.Stacktrace).c_str());
                    }
                    if (ImGui::IsItemHovered() && !logInfo.Stacktrace.empty())
                    {
                        ImGui::SetTooltip("%s", logInfo.Stacktrace.c_str());
                    }
                    ImGui::SameLine();
                    {
                        ImGuiUtils::StyleColorScope sc(ImGuiCol_Text, ImGuiUtils::GetColor(m_colorMap[logInfo.Type]));
                        ImGui::TextUnformatted(logInfo.Message.c_str());
                    }
                }
            }
            else
            {
                auto logInfos = Log::GetCollapsedLogInfos();

                // 将日志信息提取到一个可排序的容器中
                std::vector<CollapsedLogInfo> logInfoList;
                for (const auto &[source, logInfo] : logInfos)
                {
                    if (!ShouldSkip(logInfo))
                    {
                        logInfoList.push_back(logInfo);
                    }
                }
                // 按时间排序
                std::sort(logInfoList.begin(), logInfoList.end(), [](const LogInfo &a, const LogInfo &b) {
                    return a.Time < b.Time; // 升序排序
                });

                for (const CollapsedLogInfo &logInfo : logInfoList)
                {
                    if (ShouldSkip(logInfo))
                        continue;
                    std::string label = FormatTimePoint(logInfo.Time) + " -";
                    if (ImGui::Selectable(label.c_str(), false))
                    {
                        ImGui::SetClipboardText((logInfo.Message + "\n" + logInfo.Stacktrace).c_str());
                    }
                    if (ImGui::IsItemHovered() && !logInfo.Stacktrace.empty())
                    {
                        ImGui::SetTooltip("%s", logInfo.Stacktrace.c_str());
                    }
                    ImGui::SameLine();
                    {
                        ImGuiUtils::StyleColorScope sc(ImGuiCol_Text, ImGuiUtils::GetColor(m_colorMap[logInfo.Type]));
                        ImGui::TextUnformatted(logInfo.Message.c_str());
                    }
                    if (logInfo.Count > 1)
                    {
                        ImGui::SameLine();
                        ImGui::TextDisabled("(%d)", logInfo.Count);
                    }
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
