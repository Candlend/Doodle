#pragma once

#include "pch.h"
#include <imgui.h>

#include <ImGuizmo.h>

#include "Application.h"
#include "EditorPanel.h"
#include "Renderer.h"

namespace Doodle
{

class DOO_API DebugPanel : public EditorPanel
{
public:
    PANEL_CLASS_TYPE(Debug)
    void OnUpdate() override;

    void OnPanelLayout() override;

private:
    bool m_useWireframe = false;
};

} // namespace Doodle
