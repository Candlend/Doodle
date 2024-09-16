#pragma once

#include "pch.h"

#include "EditorPanel.h"

namespace Doodle
{

class DOO_API SceneSettingsPanel : public EditorPanel
{
public:
    PANEL_CLASS_TYPE(SceneSettings)
    void OnUpdate() override;

    void OnPanelLayout() override;
};

} // namespace Doodle