#include "DebugPanel.h"

namespace Doodle
{

void DebugPanel::OnPanelLayout()
{
    ImGuiUtils::ReadOnlyInputText("Time", "{}", Application::Time::GetTime());
    ImGuiUtils::ReadOnlyInputText("FPS", "{}", Application::Time::GetFPS());
    ImGui::Checkbox("Wireframe Mode", &m_useWireframe);
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
