#include "HierarchyPanel.h"
#include "Component.h"
#include "Entity.h"
#include "PanelManager.h"
#include "SceneManager.h"
#include "SelectionManager.h"
#include "imgui.h"

namespace Doodle
{

void HierarchyPanel::OnPanelLayout()
{
    auto scene = SceneManager::Get()->GetActiveScene();
    auto entities = scene->GetEntities();
    for (auto &entity : entities)
    {
        if (ImGui::Selectable(entity.GetComponent<TagComponent>().Tag.c_str(),
                              SelectionManager::IsSelected(SelectionContext::Global, entity.GetUUID())))
        {
            SelectionManager::DeselectAll(SelectionContext::Global);
            SelectionManager::Select(SelectionContext::Global, entity.GetUUID());
            ImGui::SetWindowFocus("Viewport");
        }
    }

    // 右键菜单
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::MenuItem("Create Empty Entity"))
        {
            auto entity = scene->CreateEntity("Empty Entity");
        }

        if (ImGui::BeginMenu("Create Primitives"))
        {
            if (ImGui::MenuItem("Quad"))
            {
                auto entity = scene->CreateEntity("Quad");
                entity.AddComponent<MeshComponent>(Mesh::GetQuad());
                entity.AddComponent<MaterialComponent>(StandardMaterial::Create());
            }

            if (ImGui::MenuItem("Cube"))
            {
                auto entity = scene->CreateEntity("Cube");
                entity.AddComponent<MeshComponent>(Mesh::GetCube());
                entity.AddComponent<MaterialComponent>(StandardMaterial::Create());
            }

            if (ImGui::MenuItem("Sphere"))
            {
                auto entity = scene->CreateEntity("Sphere");
                entity.AddComponent<MeshComponent>(Mesh::GetSphere());
                entity.AddComponent<MaterialComponent>(StandardMaterial::Create());
            }

            if (ImGui::MenuItem("Plane"))
            {
                auto entity = scene->CreateEntity("Plane");
                entity.AddComponent<MeshComponent>(Mesh::GetPlane());
                entity.AddComponent<MaterialComponent>(StandardMaterial::Create());
            }

            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }
}

void HierarchyPanel::OnUpdate()
{
}

} // namespace Doodle
