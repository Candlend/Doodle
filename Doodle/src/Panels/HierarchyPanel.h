#pragma once

#include "pch.h"
#include <imgui.h>

#include <ImGuizmo.h>

#include "Application.h"
#include "EditorPanel.h"
#include "Renderer.h"

namespace Doodle
{

class DOO_API HierarchyPanel : public EditorPanel
{
public:
    HierarchyPanel() : EditorPanel("Hierarchy")
    {
    }
    void OnUpdate() override;

    void OnPanelLayout() override;

private:
    bool m_useWireframe = false; // TODO 需要移去其他地方
};

} // namespace Doodle
