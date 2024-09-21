#include "EditorCamera.h"
#include "KeyCode.h"
#include "UUID.h"
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
    ImGui::ShowDemoWindow();
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

    ImGuizmo::DrawGrid(glm::value_ptr(view), glm::value_ptr(projection), glm::value_ptr(glm::mat4(1.0f)),
                       100.f); // 100x100 grid

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
        glm::mat4 model = transform.GetTransformMatrix();

        if (ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), s_Operation, s_Mode,
                                 glm::value_ptr(model)))
        {
            if (auto parent = entity.GetParent())
            {
                auto &parentTransform = parent.GetComponent<TransformComponent>();
                model = glm::inverse(parentTransform.GetTransformMatrix()) * model;
            }

            glm::vec3 translation;
            glm::vec3 rotation;
            glm::vec3 scale;
            DecomposeTransform(model, translation, rotation, scale);

            transform.SetLocalPosition(translation);
            transform.SetLocalRotation(rotation);
            transform.SetLocalScale(scale);
        }

        if (Input::IsKeyPressed(KeyCode::F))
        {
            editorCamera->Focus(transform.GetPosition());
        }
    }

    auto childFlags = ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_FrameStyle;
    auto windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_UnsavedDocument |
                       ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoNav;

    std::string overlayText = "FPS: " + std::to_string(ImGui::GetIO().Framerate);
    float padding = 8.0f;
    ImVec2 overlayPos;
    overlayPos.x = m_panelData.CursorPos.x + padding;
    overlayPos.y = m_panelData.CursorPos.y + padding;
    ImGui::SetCursorPos(overlayPos);
    ImVec2 overlaySize = ImGui::CalcTextSize(overlayText.c_str()) + ImVec2(padding, padding);

    ImGui::BeginChild("ViewportOverlay", overlaySize, childFlags, windowFlags);
    ImGui::TextUnformatted(overlayText.c_str());
    ImGui::EndChild();
}

void ViewportPanel::OnLayout()
{
    ImGuiUtils::StyleVarScope scope(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    EditorPanel::OnLayout();
}

} // namespace Doodle
