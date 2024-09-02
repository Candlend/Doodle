#include "DebugPanel.h"

namespace Doodle
{

void DebugPanel::OnPanelLayout()
{
    ImGui::LabelText("时间", "%f", Application::Time::GetTime());
    ImGui::LabelText("帧率", "%f", Application::Time::GetFPS());
    ImGui::Checkbox("线框模式", &m_useWireframe);
}

void DebugPanel::OnUpdate()
{
    Renderer::UseWireframe(m_useWireframe);
    Renderer::Clear();
}

} // namespace Doodle
