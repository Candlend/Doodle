#include "ImGuiPanel.h"

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
        auto focusedPanelID = PanelManager::Get().GetFocusedPanelID();
        if (focusedPanelID != 0 && focusedPanelID != currentData.ID)
        {
            uint32_t prevFocusedPanelID = PanelManager::Get().GetFocusedPanelID();
            PanelData *prevFocusedPanelData = PanelManager::Get().GetPanelData(prevFocusedPanelID);
            if (prevFocusedPanelData && prevFocusedPanelData->DockID == currentData.DockID)
            {
                prevFocusedPanelData->Appeared = false;
            }
        }
        PanelManager::Get().SetFocusedPanelID(currentData.ID);
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

ImGuiPanel::ImGuiPanel()
{
    EventManager::Get().AddListener<AppLayoutEvent>(this, &ImGuiPanel::OnLayout);
    EventManager::Get().AddListener<AppUpdateEvent>(this, &ImGuiPanel::OnUpdate);
}

ImGuiPanel::~ImGuiPanel()
{
    EventManager::Get().RemoveListener<AppLayoutEvent>(this, &ImGuiPanel::OnLayout);
    EventManager::Get().RemoveListener<AppUpdateEvent>(this, &ImGuiPanel::OnUpdate);
}

void ImGuiPanel::OnLayout()
{
    ImGuiUtils::WindowScope scope(m_panelName.c_str(), &m_open, m_flags);
    uint32_t id = ImGui::GetID(m_panelName.c_str());
    m_panelData = PanelData::GetCurrentPanelData(id, m_flags, m_open);
    PanelManager::Get().SetPanelData(id, m_panelData);
    OnPanelLayout();
}

} // namespace Doodle