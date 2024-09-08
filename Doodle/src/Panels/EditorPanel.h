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
        return Opened && Focused && !Collapsed;
    }

    static PanelData GetCurrentPanelData(uint32_t id, ImGuiWindowFlags flags, bool opened);
};

class DOO_API EditorPanel
{
public:
    EditorPanel(ImGuiWindowFlags flags = ImGuiWindowFlags_None);
    ~EditorPanel();

    void OnLayout();

    virtual void OnUpdate() = 0;
    virtual void OnPanelLayout() = 0;
    virtual const char *GetName() const = 0;

    uint32_t GetWidth() const
    {
        return m_panelData.PanelSize.x;
    }

    uint32_t GetHeight() const
    {
        return m_panelData.PanelSize.y;
    }

    bool IsShowOnTop() const
    {
        return m_panelData.IsShowOnTop();
    }

protected:
    PanelData m_panelData;
    bool m_open = true;
    ImGuiWindowFlags m_flags = ImGuiWindowFlags_None;
};

#define PANEL_CLASS_TYPE(type)                                                                                         \
    static const char *GetStaticName()                                                                                 \
    {                                                                                                                  \
        return #type;                                                                                                  \
    }                                                                                                                  \
    virtual const char *GetName() const override                                                                       \
    {                                                                                                                  \
        return GetStaticName();                                                                                        \
    }

} // namespace Doodle