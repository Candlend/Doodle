#pragma once

#include "EventManager.h"
#include "SceneEvent.h"
#include "pch.h"

#include "ImGuiPanel.h"

namespace Doodle
{
class DOO_API InspectorPanel : public ImGuiPanel
{
public:
    InspectorPanel();
    void OnUpdate() override;

    void OnPanelLayout() override;
};

} // namespace Doodle