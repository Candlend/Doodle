#include "DebugPanel.h"
#include "RenderPipeline.h"
#include "imgui.h"

namespace Doodle
{

void DebugPanel::OnPanelLayout()
{
    ImGuiUtils::ReadOnlyInputText("Time", "{}", Application::Time::GetTime());
    ImGuiUtils::ReadOnlyInputText("FPS", "{}", Application::Time::GetFPS());
    ImGui::Checkbox("Wireframe Mode", &m_useWireframe);
    auto shadowMap = RenderPipeline::Get()->GetFrameBuffer("ShadowMap");
    if (shadowMap)
    {
        if (ImGui::CollapsingHeader("Shadow Map"))
        {
            auto width = ImGui::GetContentRegionAvail().x;
            ImGui::Image(reinterpret_cast<void *>(static_cast<uintptr_t>(shadowMap->GetColorAttachmentRendererID(0))),
                         ImVec2(width / 2, width / 2), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::SameLine();
            ImGui::Image(reinterpret_cast<void *>(static_cast<uintptr_t>(shadowMap->GetDepthAttachmentRendererID())),
                         ImVec2(width / 2, width / 2), ImVec2(0, 1), ImVec2(1, 0));
        }
    }
}

void DebugPanel::OnUpdate()
{
    if (m_useWireframe)
    {
        Renderer::SetPolygonMode(PolygonModeType::Line);
    }
    else
    {
        Renderer::SetPolygonMode(PolygonModeType::Fill);
    }
}

} // namespace Doodle
