#include <imgui.h>

#include <ImGuizmo.h>

#include "CameraComponent.h"
#include "Component.h"
#include "EditorPanel.h"
#include "EventManager.h"
#include "ImGuiUtils.Feature.h"
#include "Log.h"
#include "MathUtils.h"
#include "SceneEvent.h"
#include "SceneManager.h"
#include "SelectionManager.h"
#include "ViewportPanel.h"
#include "glm/ext/vector_float3.hpp"
#include "imgui_internal.h"

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

    auto uuids = SelectionManager::GetSelections(SelectionContext::Global);

    if (uuids.size() == 1)
    {

        auto scene = SceneManager::Get()->GetActiveScene();

        auto cameraEntity = scene->GetMainCameraEntity();
        if (!cameraEntity)
        {
            return;
        }

        ImGuizmo::SetOrthographic(cameraEntity.GetComponent<CameraComponent>().Camera->GetProjectionType() ==
                                  ProjectionType::Orthographic);
        ImGuizmo::SetDrawlist(ImGui::GetCurrentWindow()->DrawList);

        ImGuizmo::SetRect(m_panelData.GetContentPos().x, m_panelData.GetContentPos().y, m_panelData.ContentSize.x,
                          m_panelData.ContentSize.y);
        auto entity = scene->GetEntity(uuids[0]);
        auto &transform = entity.GetComponent<TransformComponent>();
        glm::mat4 model = transform.GetModelMatrix();
        glm::mat4 view = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetModelMatrix());
        glm::mat4 projection = cameraEntity.GetComponent<CameraComponent>().GetProjectionMatrix();

        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), ImGuizmo::OPERATION::TRANSLATE,
                             ImGuizmo::MODE::LOCAL, glm::value_ptr(model));

        if (ImGuizmo::IsOver())
        {
            DOO_CORE_DEBUG("IsOver");
        }
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
    }
}

void ViewportPanel::OnLayout()
{
    ImGuiUtils::StyleVarScope scope(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    EditorPanel::OnLayout();
}

} // namespace Doodle
