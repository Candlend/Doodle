#include "InspectorPanel.h"
#include "Component.h"
#include "SceneManager.h"
#include "SelectionManager.h"
#include "imgui.h"

namespace Doodle
{

InspectorPanel::InspectorPanel() : ImGuiPanel("检视")
{
}

void InspectorPanel::OnUpdate()
{
}

void InspectorPanel::OnPanelLayout()
{
    auto uuids = SelectionManager::GetSelections(SelectionContext::Global);

    if (uuids.empty())
    {
        ImGui::TextDisabled("No entity selected");
        return;
    }

    for (auto &uuid : uuids)
    {
        auto components = SceneManager::Get()->GetActiveScene()->GetComponents(uuid);
        for (auto &component : components)
        {
            if (ImGui::CollapsingHeader(component->GetName().c_str()))
            {
                component->OnInspectorLayout();
            }
        }
    }
}

} // namespace Doodle