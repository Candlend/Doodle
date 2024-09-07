#pragma once

#include "pch.h"
#include <imgui.h>

#include <ImGuizmo.h>

#include "Application.h"
#include "EditorPanel.h"
#include "Renderer.h"

namespace Doodle
{

class DOO_API ViewportPanel : public EditorPanel
{
public:
    ViewportPanel() : EditorPanel("Viewport")
    {
    }
    void OnUpdate() override;

    void OnPanelLayout() override;
};

} // namespace Doodle
