#include "ViewportPanel.h"
#include "EventManager.h"
#include "ImGuiUtils.Feature.h"
#include "SceneEvent.h"
#include "imgui.h"


namespace Doodle
{

void ViewportPanel::OnUpdate()
{
    m_sceneRenderer->GetFrameBuffer()->Resize(m_panelData.PanelSize.x, m_panelData.PanelSize.y);
    static uint32_t s_CurrentViewportWidth = m_panelData.PanelSize.x;
    static uint32_t s_CurrentViewportHeight = m_panelData.PanelSize.y;

    if (s_CurrentViewportWidth != m_panelData.PanelSize.x || s_CurrentViewportHeight != m_panelData.PanelSize.y)
    {
        s_CurrentViewportWidth = m_panelData.PanelSize.x;
        s_CurrentViewportHeight = m_panelData.PanelSize.y;
        EventManager::Get()->Dispatch<ViewportResizeEvent>(s_CurrentViewportWidth, s_CurrentViewportHeight);
    }
}

void ViewportPanel::OnPanelLayout()
{
    ImGuiUtils::StyleVarScope scope(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    auto frameBuffer = m_sceneRenderer->GetFrameBuffer();
    auto textureID = frameBuffer->GetColorAttachmentRendererID();
    auto size = m_panelData.PanelSize;

    ImGui::Image(reinterpret_cast<void *>(static_cast<uintptr_t>(textureID)), ImVec2(size.x, size.y), ImVec2(0, 1),
                 ImVec2(1, 0));
}

} // namespace Doodle
