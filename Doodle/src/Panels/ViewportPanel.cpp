#include "ViewportPanel.h"
#include "EditorPanel.h"
#include "EventManager.h"
#include "ImGuiUtils.Feature.h"
#include "SceneEvent.h"
#include "imgui.h"


namespace Doodle
{

void ViewportPanel::OnUpdate()
{
    static uint32_t s_CurrentViewportWidth = m_panelData.ContentSize.x;
    static uint32_t s_CurrentViewportHeight = m_panelData.ContentSize.y;

    if (s_CurrentViewportWidth != m_panelData.ContentSize.x || s_CurrentViewportHeight != m_panelData.ContentSize.y)
    {
        s_CurrentViewportWidth = m_panelData.ContentSize.x;
        s_CurrentViewportHeight = m_panelData.ContentSize.y;
        EventManager::Get()->Dispatch<ViewportResizeEvent>(s_CurrentViewportWidth, s_CurrentViewportHeight);
    }
}

void ViewportPanel::OnPanelLayout()
{
    auto frameBuffer = m_sceneRenderer->GetFrameBuffer();
    auto textureID = frameBuffer->GetColorAttachmentRendererID();
    auto size = m_panelData.ContentSize;
    ImGui::Image(reinterpret_cast<void *>(static_cast<uintptr_t>(textureID)), ImVec2(size.x, size.y), ImVec2(0, 1),
                 ImVec2(1, 0));
}

void ViewportPanel::OnLayout()
{
    ImGuiUtils::StyleVarScope scope(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    EditorPanel::OnLayout();
}

} // namespace Doodle
