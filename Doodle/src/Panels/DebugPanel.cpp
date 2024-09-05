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
    Renderer::UseWireframe(m_useWireframe);
    Renderer::Clear();
}

} // namespace Doodle
