#include "EditorPanel.h"
#include "Log.h"

namespace Doodle
{

PanelData PanelData::GetCurrentPanelData(uint32_t id, ImGuiWindowFlags flags, bool opened)
{
    PanelData currentData;
    currentData.ID = id;
    currentData.Flags = flags;
    currentData.Opened = opened;
    currentData.PanelPos = glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
    currentData.PanelSize = glm::vec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
    currentData.CursorPos = glm::vec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y);
    currentData.ContentSize = glm::vec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
    currentData.DockID = ImGui::GetWindowDockID();
    currentData.Collapsed = ImGui::IsWindowCollapsed();
    currentData.Focused = ImGui::IsWindowFocused();
    currentData.Hovered = ImGui::IsWindowHovered();
    currentData.Docked = ImGui::IsWindowDocked();
    currentData.Appearing = ImGui::IsWindowAppearing();
    currentData.Appeared = false;
    currentData.Moving = false;
    if (currentData.Focused)
    {
        auto focusedPanelID = PanelManager::Get()->GetFocusedPanelID();
        if (focusedPanelID != 0 && focusedPanelID != currentData.ID)
        {
            uint32_t prevFocusedPanelID = PanelManager::Get()->GetFocusedPanelID();
            PanelData *prevFocusedPanelData = PanelManager::Get()->GetPanelData(prevFocusedPanelID);
            if (prevFocusedPanelData && prevFocusedPanelData->DockID == currentData.DockID)
            {
                prevFocusedPanelData->Appeared = false;
            }
        }
        PanelManager::Get()->SetFocusedPanelID(currentData.ID);
        currentData.Appeared = true;

        currentData.Moving = false;
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            ImVec2 mousePos = ImGui::GetMousePos();
            if (currentData.Flags & ImGuiWindowFlags_NoTitleBar)
            {
                if (mousePos.x >= currentData.PanelPos.x &&
                    mousePos.x <= currentData.PanelPos.x + currentData.PanelSize.x &&
                    mousePos.y >= currentData.PanelPos.y &&
                    mousePos.y <= currentData.PanelPos.y + currentData.PanelSize.y)
                {
                    currentData.Moving = true;
                }
            }
            else
            {
                ImVec2 titleBarSize = ImVec2(currentData.PanelSize.x, currentData.CursorPos.y);
                if (mousePos.x >= currentData.PanelPos.x &&
                    mousePos.x <= currentData.PanelPos.x + currentData.PanelSize.x &&
                    mousePos.y >= currentData.PanelPos.y && mousePos.y <= currentData.PanelPos.y + titleBarSize.y)
                {
                    currentData.Moving = true;
                }
            }
        }
    }
    return currentData;
}

EditorPanel::EditorPanel(ImGuiWindowFlags flags) : m_flags(flags)
{
    EventManager::Get()->AddListener<AppLayoutEvent>(this, &EditorPanel::OnLayout);
    EventManager::Get()->AddListener<AppUpdateEvent>(this, &EditorPanel::OnUpdate);
}

EditorPanel::~EditorPanel()
{
    EventManager::Get()->RemoveListener<AppLayoutEvent>(this, &EditorPanel::OnLayout);
    EventManager::Get()->RemoveListener<AppUpdateEvent>(this, &EditorPanel::OnUpdate);
}

void EditorPanel::OnLayout()
{
    if (!m_open)
        return;
    if (ImGui::Begin(GetName(), &m_open, m_flags))
    {
        uint32_t id = ImGui::GetID(GetName());
        m_panelData = PanelData::GetCurrentPanelData(id, m_flags, m_open);
        PanelManager::Get()->SetPanelData(id, m_panelData);
        OnPanelLayout();
    }
    ImGui::End();
}

} // namespace Doodle