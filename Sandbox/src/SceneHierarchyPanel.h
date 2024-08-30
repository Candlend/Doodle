#pragma once

#include <imgui.h>

#include <ImGuizmo.h>
#include <glm/gtc/matrix_transform.hpp>

#include <Doodle.h>

#include "CameraController.h"
#include "ImGuiPanel.h"
#include "glm/matrix.hpp"

using namespace Doodle;

class SceneHierarchyPanel : public ImGuiPanel
{
public:
    void OnUpdate() override
    {
        Renderer::UseWireframe(m_useWireframe);
        Renderer::Clear();
    }

    void OnPanelLayout() override
    {
        ImGui::LabelText("时间", "%f", Application::Time::GetTime());
        ImGui::LabelText("帧率", "%f", Application::Time::GetFPS());
        ImGui::Checkbox("线框模式", &m_useWireframe);
    }

private:
    bool m_useWireframe = false;
    std::shared_ptr<Scene> m_activeScene;
};