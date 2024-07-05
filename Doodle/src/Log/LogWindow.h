#pragma once

#include "pch.h"
#include <imgui.h>

#include "Module.h"

namespace Doodle
{

class DOO_API LogWindow : public RegisterModule<LogWindow>
{
public:
    void Initialize() override;
    void OnUpdate() override;
    void OnLayout() override;
    void Deinitialize() override;

private:
    char m_filter[100] = "";
    bool m_showLog = true;
    bool m_showWarning = true;
    bool m_showError = true;
    int m_logCount = 0;
    int m_warningCount = 0;
    int m_errorCount = 0;
    std::map<LogType, ImU32> m_colorMap = {
        {LogType::Log, 0x00FFFF}, {LogType::Warning, 0xFFFF00}, {LogType::Error, 0xFF6666}};
};

} // namespace Doodle
