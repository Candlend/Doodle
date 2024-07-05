#pragma once

#include <imgui.h>

namespace Doodle
{
namespace ImGuiUtils
{

class ImGuiScope
{
public:
    ImGuiScope() : m_opened(true)
    {
    }
    virtual ~ImGuiScope()
    {
        Dispose();
    }
    virtual void Dispose() {};
    bool IsOpened() const
    {
        return m_opened;
    }

protected:
    bool m_opened;
};

class WindowScope : public ImGuiScope
{
public:
    explicit WindowScope(const char *strId, bool *pOpen = nullptr, ImGuiWindowFlags windowFlags = 0)
    {
        m_opened = ImGui::Begin(strId, pOpen, windowFlags);
    }
    void Dispose() override
    {
        if (m_opened)
            ImGui::End();
    }
};

class ChildWindowScope : public ImGuiScope
{
public:
    explicit ChildWindowScope(const char *strId, ImVec2 size = ImVec2(0, 0), ImGuiChildFlags childFlags = 0,
                              ImGuiWindowFlags windowFlags = 0)
    {
        m_opened = ImGui::BeginChild(strId, size, childFlags, windowFlags);
    }
    void Dispose() override
    {
        if (m_opened)
            ImGui::EndChild();
    }
};

class ComboScope : public ImGuiScope
{
public:
    explicit ComboScope(const char *label, const char *previewValue = nullptr, ImGuiComboFlags flags = 0)
    {
        m_opened = ImGui::BeginCombo(label, previewValue, flags);
    }
    void Dispose() override
    {
        if (m_opened)
            ImGui::EndCombo();
    }
};

class DisabledScope : public ImGuiScope
{
public:
    explicit DisabledScope(bool disabled = true)
    {
        ImGui::BeginDisabled(disabled);
    }
    void Dispose() override
    {
        ImGui::EndDisabled();
    }
};

class DragDropSourceScope : public ImGuiScope
{
public:
    explicit DragDropSourceScope(ImGuiDragDropFlags flags = 0)
    {
        m_opened = ImGui::BeginDragDropSource(flags);
    }
    void Dispose() override
    {
        if (m_opened)
            ImGui::EndDragDropSource();
    }
};

class DragDropTargetScope : public ImGuiScope
{
public:
    DragDropTargetScope()
    {
        m_opened = ImGui::BeginDragDropTarget();
    }
    void Dispose() override
    {
        if (m_opened)
            ImGui::EndDragDropTarget();
    }
};

class GroupScope : public ImGuiScope
{
public:
    GroupScope()
    {
        ImGui::BeginGroup();
    }
    void Dispose() override
    {
        ImGui::EndGroup();
    }
};

class ListBoxScope : public ImGuiScope
{
public:
    explicit ListBoxScope(const char *label, ImVec2 size = ImVec2(0, 0))
    {
        m_opened = ImGui::BeginListBox(label, size);
    }
    void Dispose() override
    {
        if (m_opened)
            ImGui::EndListBox();
    }
};

class MainMenuBarScope : public ImGuiScope
{
public:
    MainMenuBarScope()
    {
        m_opened = ImGui::BeginMainMenuBar();
    }
    void Dispose() override
    {
        if (m_opened)
            ImGui::EndMainMenuBar();
    }
};

class MenuScope : public ImGuiScope
{
public:
    explicit MenuScope(const char *label, bool enabled = true)
    {
        m_opened = ImGui::BeginMenu(label, enabled);
    }
    void Dispose() override
    {
        if (m_opened)
            ImGui::EndMenu();
    }
};

class MenuBarScope : public ImGuiScope
{
public:
    MenuBarScope()
    {
        m_opened = ImGui::BeginMenuBar();
    }
    void Dispose() override
    {
        if (m_opened)
            ImGui::EndMenuBar();
    }
};

class PopupScope : public ImGuiScope
{
public:
    explicit PopupScope(const char *strId, ImGuiWindowFlags flags = 0)
    {
        m_opened = ImGui::BeginPopup(strId, flags);
    }
    void Dispose() override
    {
        if (m_opened)
            ImGui::EndPopup();
    }
};

class PopupContextItemScope : public ImGuiScope
{
public:
    explicit PopupContextItemScope(const char *strId = nullptr,
                                   ImGuiPopupFlags popupFlags = ImGuiPopupFlags_MouseButtonRight)
    {
        m_opened = ImGui::BeginPopupContextItem(strId, popupFlags);
    }
    void Dispose() override
    {
        if (m_opened)
            ImGui::EndPopup();
    }
};

class PopupContextVoidScope : public ImGuiScope
{
public:
    explicit PopupContextVoidScope(const char *strId = nullptr,
                                   ImGuiPopupFlags popupFlags = ImGuiPopupFlags_MouseButtonRight)
    {
        m_opened = ImGui::BeginPopupContextVoid(strId, popupFlags);
    }
    void Dispose() override
    {
        if (m_opened)
            ImGui::EndPopup();
    }
};

class PopupContextWindowScope : public ImGuiScope
{
public:
    explicit PopupContextWindowScope(const char *strId = nullptr,
                                     ImGuiPopupFlags popupFlags = ImGuiPopupFlags_MouseButtonRight)
    {
        m_opened = ImGui::BeginPopupContextWindow(strId, popupFlags);
    }
    void Dispose() override
    {
        if (m_opened)
            ImGui::EndPopup();
    }
};

class PopupModalScope : public ImGuiScope
{
public:
    PopupModalScope(const char *name, bool *pOpen, ImGuiWindowFlags flags = 0)
    {
        m_opened = ImGui::BeginPopupModal(name, pOpen, flags);
    }
    void Dispose() override
    {
        if (m_opened)
            ImGui::EndPopup();
    }
};

class TabBarScope : public ImGuiScope
{
public:
    explicit TabBarScope(const char *strId, ImGuiTabBarFlags flags = 0)
    {
        m_opened = ImGui::BeginTabBar(strId, flags);
    }
    void Dispose() override
    {
        if (m_opened)
            ImGui::EndTabBar();
    }
};

class TabItemScope : public ImGuiScope
{
public:
    explicit TabItemScope(const char *label)
    {
        m_opened = ImGui::BeginTabItem(label);
    }
    TabItemScope(const char *label, bool *pOpen, ImGuiTabItemFlags flags = 0)
    {
        m_opened = ImGui::BeginTabItem(label, pOpen, flags);
    }
    void Dispose() override
    {
        if (m_opened)
            ImGui::EndTabItem();
    }
};

class TableScope : public ImGuiScope
{
public:
    TableScope(const char *strId, int columnsCount, ImGuiTableFlags flags = 0, ImVec2 outerSize = ImVec2(0, 0),
               float innerWidth = 0.0f)
    {
        m_opened = ImGui::BeginTable(strId, columnsCount, flags, outerSize, innerWidth);
    }
    void Dispose() override
    {
        if (m_opened)
            ImGui::EndTable();
    }
};

class TooltipScope : public ImGuiScope
{
public:
    TooltipScope()
    {
        ImGui::BeginTooltip();
    }
    void Dispose() override
    {
        ImGui::EndTooltip();
    }
};

} // namespace ImGuiUtils
} // namespace Doodle