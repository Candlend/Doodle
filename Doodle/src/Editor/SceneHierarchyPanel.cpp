#include "SceneHierarchyPanel.h"

namespace Doodle
{

void SceneHierarchyPanel::OnPanelLayout()
{
    ImGui::LabelText("时间", "%f", Application::Time::GetTime());
    ImGui::LabelText("帧率", "%f", Application::Time::GetFPS());
    ImGui::Checkbox("线框模式", &m_useWireframe);
    // TODO 仅占位
}

void SceneHierarchyPanel::OnUpdate()
{
    Renderer::UseWireframe(m_useWireframe);
    Renderer::Clear();
    // TODO 仅占位
}

} // namespace Doodle
