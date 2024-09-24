#pragma once

#include "pch.h"
#include <imgui.h>

#include <ImGuizmo.h>

#include "EditorPanel.h"
#include "Entity.h"

namespace Doodle
{

class Entity;
class DOO_API HierarchyPanel : public EditorPanel
{
public:
    PANEL_CLASS_TYPE(Hierarchy)
    void OnUpdate() override;

    void OnPanelLayout() override;

private:
    void DrawEntityTree(std::vector<Entity> &entities);

    Entity m_hoveredEntity;
};

} // namespace Doodle
