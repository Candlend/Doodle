#pragma once

#include "SceneRenderer.h"
#include "pch.h"
#include <imgui.h>

#include <ImGuizmo.h>
#include <memory>

#include "Application.h"
#include "EditorPanel.h"
#include "Renderer.h"

namespace Doodle
{

class DOO_API ViewportPanel : public EditorPanel
{
public:
    PANEL_CLASS_TYPE(Viewport)
    ViewportPanel()
    {
        m_sceneRenderer = SceneRenderer::Get();
    }
    void OnUpdate() override;

    void OnPanelLayout() override;
    void OnLayout() override;

private:
    SceneRenderer *m_sceneRenderer;
};

} // namespace Doodle
