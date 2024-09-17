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

    if (ImGui::CollapsingHeader("Shadow"))
    {
        ImGui::DragFloat("Bias", &sceneData.ShadowBias, 0.0001f, 0.0f, 0.1f);
        ImGui::DragFloat("Normal Bias", &sceneData.ShadowNormalBias, 0.0001f, 0.0f, 0.1f);
    }
}

} // namespace Doodle