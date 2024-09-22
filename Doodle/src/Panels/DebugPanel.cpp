#include "DebugPanel.h"
#include "RenderPipeline.h"
#include "imgui.h"

namespace Doodle
{

void DebugPanel::OnPanelLayout()
{
    ImGui::BeginDisabled();
    ImGuiUtils::ReadOnlyInputFloat("Time", Application::Time::GetTime());
    ImGuiUtils::ReadOnlyInputInt("FPS", Application::Time::GetFPS());
    ImGui::EndDisabled();
    ImGui::Checkbox("Wireframe Mode", &m_useWireframe);
    auto width = ImGui::GetContentRegionAvail().x;

    for (auto &frameBuffer : RenderPipeline::Get()->GetFrameBuffers())
    {
        if (ImGui::CollapsingHeader(frameBuffer.first.c_str()))
        {
            auto spec = frameBuffer.second->GetSpecification();
            auto height = width * spec.Height / spec.Width;
            int colorAttachmentIndex = 0;
            for (auto &attachment : spec.Attachments.Attachments)
            {
                if (attachment.TextureFormat == FramebufferTextureFormat::Depth)
                {
                    ImGui::Image(reinterpret_cast<void *>(
                                     static_cast<uintptr_t>(frameBuffer.second->GetDepthAttachmentRendererID())),
                                 ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));
                }
                else
                {
                    ImGui::Image(reinterpret_cast<void *>(static_cast<uintptr_t>(
                                     frameBuffer.second->GetColorAttachmentRendererID(colorAttachmentIndex++))),
                                 ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));
                }
            }
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
