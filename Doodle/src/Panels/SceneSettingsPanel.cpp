#include "SceneSettingsPanel.h"
#include "Component.h"
#include "SceneManager.h"
#include "SelectionManager.h"
#include "imgui.h"

namespace Doodle
{

void SceneSettingsPanel::OnUpdate()
{
}

void SceneSettingsPanel::OnPanelLayout()
{
    auto scene = SceneManager::Get()->GetActiveScene();
    if (!scene)
    {
        ImGui::TextDisabled("No active scene");
        return;
    }

    auto &sceneData = scene->GetData();

    if (ImGui::CollapsingHeader("Environment"))
    {
        ImGui::DragFloat("Intensity", &sceneData.EnvironmentData.Intensity, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("Rotation", &sceneData.EnvironmentData.Rotation, 0.1f, 0.0f, 360.0f);
    }
}

} // namespace Doodle