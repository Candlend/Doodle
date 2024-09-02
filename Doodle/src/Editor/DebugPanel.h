#pragma once

#include "pch.h"
#include <imgui.h>

#include <ImGuizmo.h>

#include "Application.h"
#include "ImGuiPanel.h"
#include "Renderer.h"

namespace Doodle
{

class DOO_API DebugPanel : public ImGuiPanel
{
public:
    DebugPanel() : ImGuiPanel("Debug")
    {
    }
    void OnUpdate() override;

    void OnPanelLayout() override;

private:
    bool m_useWireframe = false;
};

} // namespace Doodle
