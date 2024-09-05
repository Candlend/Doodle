#pragma once

#include "EventManager.h"
#include "SceneEvent.h"
#include "pch.h"

#include "EditorPanel.h"

namespace Doodle
{
class DOO_API InspectorPanel : public EditorPanel
{
public:
    InspectorPanel();
    void OnUpdate() override;

    void OnPanelLayout() override;
};

} // namespace Doodle