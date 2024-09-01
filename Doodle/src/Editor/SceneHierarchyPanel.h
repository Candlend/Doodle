#pragma once

#include "pch.h"
#include <imgui.h>

#include <ImGuizmo.h>

#include "Application.h"
#include "ImGuiPanel.h"
#include "Renderer.h"

namespace Doodle
{

class DOO_API SceneHierarchyPanel : public ImGuiPanel
{
public:
    SceneHierarchyPanel() : ImGuiPanel("层级")
    {
    }
    void OnUpdate() override;

    void OnPanelLayout() override;

private:
    bool m_useWireframe = false; // TODO 需要移去其他地方
};

} // namespace Doodle
