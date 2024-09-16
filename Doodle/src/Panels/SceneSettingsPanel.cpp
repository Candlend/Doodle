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
        ImGui::SliderFloat("Intensity", &sceneData.EnvironmentData.Intensity, 0.0f, 5.0f);
        ImGui::SliderFloat("Rotation", &sceneData.EnvironmentData.Rotation, 0.0f, 360.0f);
    }
}

} // namespace Doodle