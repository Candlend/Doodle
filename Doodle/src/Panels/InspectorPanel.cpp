#include <imgui.h>

#include "Component.h"
#include "Entity.h"
#include "InspectorPanel.h"
#include "SceneManager.h"
#include "SelectionManager.h"

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
    bool enabled = true;
    // TODO 添加enabled开关
    // ImGui::Checkbox("##Enabled", &enabled);
    // ImGui::SameLine();
    auto uuid = uuids[0];
    auto entity = SceneManager::Get()->GetActiveScene()->GetEntity(uuid);
    // 获取窗口的宽度
    float windowWidth = ImGui::GetContentRegionAvail().x;

    // 设置下一个 InputText 的宽度为窗口的可用宽度
    ImGui::SetNextItemWidth(windowWidth);
    ImGuiUtils::InputText("##Name", entity.GetComponent<NameComponent>().Name);
    auto components = SceneManager::Get()->GetActiveScene()->GetComponents(uuid);

    for (auto &component : components)
    {
        std::string name = component->GetName();
        if (name == "ID" || name == "Name")
        {
            continue;
        }
        if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            component->OnInspectorLayout();
        }
    }

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
        AddComponentButton<AreaLightComponent>(entity);
        ImGui::EndPopup();
    }
}

} // namespace Doodle