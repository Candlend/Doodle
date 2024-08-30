#pragma once

#include "imgui.h"
#include "pch.h"

#include "ApplicationEvent.h"
#include "EventManager.h"
#include "ImGuiUtils.h"
#include "PanelManager.h"
#include "Singleton.h"
#include <cstdint>

namespace Doodle
{

struct DOO_API PanelData
{
    uint32_t ID;
    ImGuiWindowFlags Flags;
    bool Opened;
    glm::vec2 PanelPos;
    glm::vec2 PanelSize;
    glm::vec2 CursorPos;
    glm::vec2 ContentSize;
    uint32_t DockID;
    bool Collapsed;
    bool Focused;
    bool Hovered;
    bool Docked;
    bool Appearing;
    bool Appeared;
    bool Moving;

    inline glm::vec2 GetContentPos() const
    {
        return PanelPos + CursorPos;
    }

    inline bool IsShowOnTop() const
    {
        return Opened && Focused && !Collapsed && !Docked;
    }

    static PanelData GetCurrentPanelData(uint32_t id, ImGuiWindowFlags flags, bool opened);
};

class DOO_API ImGuiPanel
{
public:
    ImGuiPanel(const std::string &panelName, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
    ~ImGuiPanel();

    void OnLayout();

    virtual void OnUpdate() = 0;
    virtual void OnPanelLayout() = 0;

    inline std::string GetPanelName()
    {
        return m_panelName;
    }

protected:
    PanelData m_panelData;
    std::string m_panelName;
    bool m_open = true;
    ImGuiWindowFlags m_flags = ImGuiWindowFlags_None;
};

} // namespace Doodle