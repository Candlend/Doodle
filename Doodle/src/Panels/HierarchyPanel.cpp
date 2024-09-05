#include "Component.h"
#include "Entity.h"
#include "HierarchyPanel.h"
#include "SceneManager.h"
#include "SelectionManager.h"


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
        }
    }
}

void HierarchyPanel::OnUpdate()
{
}

} // namespace Doodle
