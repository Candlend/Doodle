#pragma once

#include "pch.h"
#include <imgui.h>

namespace Doodle
{

class DOO_API ImGuiPopupModal
{
public:
    virtual const char *GetModalName() const = 0;
    virtual void OnLayOut() = 0;
    virtual void Confirm() = 0;
    virtual void Cancel()
    {
    }

    bool IsOpened() const
    {
        return m_opened;
    }

protected:
    bool m_opened = false;

public:
    virtual void Open()
    {
        m_opened = true;
        ImGui::OpenPopup(GetModalName());
    }

    void Draw()
    {
        auto *viewport = ImGui::GetMainViewport();
        ImVec2 pos = viewport->Pos;
        ImVec2 size = viewport->Size;
        ImVec2 center = ImVec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal(GetModalName(), &m_opened, ImGuiWindowFlags_AlwaysAutoResize))
        {
            OnLayOut();
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();

            if (ImGui::Button("Confirm", ImVec2(120, 0)))
            {
                Confirm();
                ImGui::CloseCurrentPopup();
                m_opened = false;
            }

            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                Cancel();
                ImGui::CloseCurrentPopup();
                m_opened = false;
            }
        }
    }
};
} // namespace Doodle
