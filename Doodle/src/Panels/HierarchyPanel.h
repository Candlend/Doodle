#pragma once

#include "Entity.h"
#include "pch.h"
#include <cstdint>
#include <imgui.h>

#include <ImGuizmo.h>

#include "Application.h"
#include "EditorPanel.h"
#include "Renderer.h"

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
