#include "InspectorPanel.h"
#include "Component.h"
#include "Entity.h"
#include "MaterialComponent.h"
#include "Renderable.h"
#include "SceneManager.h"
#include "SelectionManager.h"
#include "imgui.h"

namespace Doodle
{

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

    if (uuids.size() > 1)
    {
        ImGui::TextDisabled("Multiple entities selected");
        return;
    }
    auto uuid = uuids[0];
    auto components = SceneManager::Get()->GetActiveScene()->GetComponents(uuid);
    for (auto &component : components)
    {
        if (ImGui::CollapsingHeader(component->GetName()))
        {
            component->OnInspectorLayout();
        }
    }
    auto entity = SceneManager::Get()->GetActiveScene()->GetEntity(uuid);

    ImGui::Separator();
    ImGui::Spacing();
    if (ImGuiUtils::SizedButton("Add Component"))
    {
        ImGui::OpenPopup("AddComponentPopup");
    }

    if (ImGui::BeginPopup("AddComponentPopup"))
    {
        AddComponentButton<CameraComponent>(entity);
        AddComponentButton<DirectionalLightComponent>(entity);
        AddComponentButton<PointLightComponent>(entity);
        AddComponentButton<SpotLightComponent>(entity);
        ImGui::EndPopup();
    }
}

} // namespace Doodle