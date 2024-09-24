#pragma once

#include "pch.h"

#include "EditorPanel.h"
#include "SceneRenderer.h"

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
    std::shared_ptr<SceneRenderer> m_sceneRenderer;
};

} // namespace Doodle
