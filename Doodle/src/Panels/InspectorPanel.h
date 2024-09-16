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
    PANEL_CLASS_TYPE(Inspector)
    void OnUpdate() override;

    void OnPanelLayout() override;

    template <typename T> void AddComponentButton(Entity &entity)
    {
        if (entity.HasComponent<T>())
            return;
        if (ImGui::MenuItem(T::GetStaticName()))
        {
            entity.AddComponent<T>();
        }
    }
};

} // namespace Doodle