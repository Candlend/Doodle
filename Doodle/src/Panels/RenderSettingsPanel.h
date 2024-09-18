#pragma once

#include "pch.h"

#include "EditorPanel.h"

namespace Doodle
{

class DOO_API RenderSettingsPanel : public EditorPanel
{
public:
    PANEL_CLASS_TYPE(RenderSettings)
    void OnUpdate() override;

    void OnPanelLayout() override;
};

} // namespace Doodle