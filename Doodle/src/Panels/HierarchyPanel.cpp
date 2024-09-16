#include "HierarchyPanel.h"
#include "Component.h"
#include "Entity.h"
#include "PanelManager.h"
#include "SceneManager.h"
#include "SelectionManager.h"
#include "imgui.h"

namespace Doodle
{

void HierarchyPanel::OnPanelLayout()
{
    auto scene = SceneManager::Get()->GetActiveScene();
    auto entities = scene->GetEntities();
    for (auto &entity : entities)
    {
        if (ImGui::Selectable(entity.GetComponent<TagComponent>().Tag.c_str(),
                              SelectionManager::IsSelected(SelectionContext::Global, entity.GetUUID())))
        {
            SelectionManager::DeselectAll(SelectionContext::Global);
            SelectionManager::Select(SelectionContext::Global, entity.GetUUID());
            ImGui::SetWindowFocus("Viewport");
        }
    }

    // 右键菜单
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::MenuItem("Create Empty Entity"))
        {
            auto entity = scene->CreateEntity("Empty Entity");
        }
        ImGui::EndPopup();
    }
}

void HierarchyPanel::OnUpdate()
{
}

} // namespace Doodle
