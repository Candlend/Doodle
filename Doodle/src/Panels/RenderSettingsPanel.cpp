#include "Component.h"
#include "RenderPass.h"
#include "RenderSettingsPanel.h"
#include "SceneManager.h"
#include "SelectionManager.h"
#include "imgui.h"


namespace Doodle
{

void RenderSettingsPanel::OnUpdate()
{
}

void RenderSettingsPanel::OnPanelLayout()
{
    auto scene = SceneManager::Get()->GetActiveScene();
    if (!scene)
    {
        ImGui::TextDisabled("No active scene");
        return;
    }

    for (auto &renderPass : RenderPipeline::Get()->GetRenderPasses())
    {
        if (ImGui::CollapsingHeader(renderPass.first.c_str()))
        {
            renderPass.second->OnLayout();
        }
    }
}

} // namespace Doodle