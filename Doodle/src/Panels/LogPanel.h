#pragma once

#include "pch.h"
#include <imgui.h>

#include "EditorPanel.h"

namespace Doodle
{

class DOO_API LogPanel : public EditorPanel
{
public:
    PANEL_CLASS_TYPE(Log)
    void OnUpdate() override;
    void OnPanelLayout() override;

private:
    char m_filter[100] = "";
    bool m_collapse = true;
    bool m_showLog = true;
    bool m_showWarning = true;
    bool m_showError = true;
    int m_logCount = 0;
    int m_warningCount = 0;
    int m_errorCount = 0;
    std::map<LogType, ImU32> m_colorMap = {{LogType::Trace, 0xFFFFFF},
                                           {LogType::Debug, 0x30B2E6},
                                           {LogType::Info, 0x70D879},
                                           {LogType::Warning, 0xFFFF00},
                                           {LogType::Error, 0xFF6666}};

    bool ShouldSkip(const LogInfo &logInfo);
};

} // namespace Doodle
