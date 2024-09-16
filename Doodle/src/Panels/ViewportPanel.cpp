#include "EditorCamera.h"
#include "KeyCode.h"
#include "pch.h"
#include <glm/ext/vector_float3.hpp>
#include <imgui.h>
#include <imgui_internal.h>

#include <ImGuizmo.h>

#include "CameraComponent.h"
#include "Component.h"
#include "EditorPanel.h"
#include "EventManager.h"
#include "ImGuiUtils.Feature.h"
#include "Input.h"
#include "MathUtils.h"
#include "SceneEvent.h"
#include "SceneManager.h"
#include "SelectionManager.h"
#include "ViewportPanel.h"

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
    ImGui::SetKeyOwner(ImGuiMod_Alt, m_panelData.ID);

    auto frameBuffer = m_sceneRenderer->GetFrameBuffer();
    auto textureID = frameBuffer->GetColorAttachmentRendererID();
    auto size = m_panelData.ContentSize;
    ImGui::Image(reinterpret_cast<void *>(static_cast<uintptr_t>(textureID)), ImVec2(size.x, size.y), ImVec2(0, 1),
                 ImVec2(1, 0));
    if (SceneManager::Get()->GetState() != SceneState::Editor)
        return;

    auto scene = SceneManager::Get()->GetActiveScene();

    auto *editorCamera = EditorCamera::Get();

    ImGuizmo::SetOrthographic(editorCamera->GetProjectionType() == ProjectionType::Orthographic);
    ImGuizmo::SetDrawlist(ImGui::GetCurrentWindow()->DrawList);

    ImGuizmo::SetRect(m_panelData.GetContentPos().x, m_panelData.GetContentPos().y, m_panelData.ContentSize.x,
                      m_panelData.ContentSize.y);
    glm::mat4 view = editorCamera->GetViewMatrix();
    glm::mat4 projection = editorCamera->GetProjectionMatrix();

    static ImGuizmo::OPERATION s_Operation = ImGuizmo::OPERATION::TRANSLATE;
    static ImGuizmo::MODE s_Mode = ImGuizmo::MODE::LOCAL;
    if (!Input::IsMouseButtonDown(MouseButton::Right))
    {
        if (Input::IsKeyPressed(KeyCode::W))
        {
            s_Operation = ImGuizmo::OPERATION::TRANSLATE;
        }
        else if (Input::IsKeyPressed(KeyCode::E))
        {
            s_Operation = ImGuizmo::OPERATION::ROTATE;
        }
        else if (Input::IsKeyPressed(KeyCode::R))
        {
            s_Operation = ImGuizmo::OPERATION::SCALE;
        }

        if (Input::IsKeyPressed(KeyCode::Tab))
        {
            s_Mode = s_Mode == ImGuizmo::MODE::LOCAL ? ImGuizmo::MODE::WORLD : ImGuizmo::MODE::LOCAL;
        }
    }

    auto uuids = SelectionManager::GetSelections(SelectionContext::Global);

    if (uuids.size() == 1)
    {
        auto entity = scene->GetEntity(uuids[0]);
        auto &transform = entity.GetComponent<TransformComponent>();
        glm::mat4 model = transform.GetModelMatrix();

        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), s_Operation, s_Mode,
                             glm::value_ptr(model));

        if (ImGuizmo::IsUsing())
        {
            glm::vec3 translation;
            glm::vec3 rotation;
            glm::vec3 scale;
            DecomposeTransform(model, translation, rotation, scale);

            transform.Position = translation;
            transform.Rotation = rotation;
            transform.Scale = scale;
        }

        if (Input::IsKeyPressed(KeyCode::F))
        {
            editorCamera->Focus(transform.Position);
        }
    }
}

void ViewportPanel::OnLayout()
{
    ImGuiUtils::StyleVarScope scope(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    EditorPanel::OnLayout();
}

} // namespace Doodle
