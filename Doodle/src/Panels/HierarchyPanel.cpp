#include "HierarchyPanel.h"
#include "Component.h"
#include "EditorCamera.h"
#include "Entity.h"
#include "Input.h"
#include "PanelManager.h"
#include "SceneManager.h"
#include "SelectionManager.h"
#include "imgui.h"
#include "imgui_internal.h"

namespace Doodle
{

void HierarchyPanel::DrawEntityTree(std::vector<Entity> &entities)
{
    for (const auto &entity : entities)
    {
        // Get the tag for the current entity
        const auto &tag = entity.GetComponent<TagComponent>().Tag;
        // Create a tree node for the entity
        bool isSelected = SelectionManager::IsSelected(SelectionContext::Global, entity.GetUUID());
        auto children = entity.GetChildren();
        // 有没有子节点
        auto flags = ImGuiTreeNodeFlags_OpenOnArrow | (isSelected ? ImGuiTreeNodeFlags_Selected : 0) |
                     (children.empty() ? ImGuiTreeNodeFlags_Leaf : 0);

        bool opened = ImGui::TreeNodeEx(tag.c_str(), flags);
        if (ImGui::IsItemHovered() && ImGui::IsMouseDown(1))
        {
            m_hoveredEntity = entity;
        }
        // Handle selection
        if (ImGui::IsItemClicked())
        {
            SelectionManager::DeselectAll(SelectionContext::Global);
            SelectionManager::Select(SelectionContext::Global, entity.GetUUID());
            ImGui::SetWindowFocus("Viewport");
            if (ImGui::IsMouseDoubleClicked(0))
            {
                EditorCamera::Get()->Focus(entity.GetComponent<TransformComponent>().GetPosition());
            }
        }

        if (opened)
        {
            DrawEntityTree(children);
            ImGui::TreePop();
        }
    }
}
void HierarchyPanel::OnPanelLayout()
{
    if (ImGui::IsMouseClicked(1))
    {
        m_hoveredEntity = Entity();
    }
    auto scene = SceneManager::Get()->GetActiveScene();
    auto entities = scene->GetEntities();
    std::vector<Entity> rootEntities;
    for (const auto &entity : entities)
    {
        if (entity.GetParentUUID() == UUID::Nil())
        {
            rootEntities.push_back(entity);
        }
    }
    DrawEntityTree(rootEntities);

    // 右键菜单
    if (ImGui::BeginPopupContextWindow())
    {
        if (m_hoveredEntity)
        {
            if (ImGui::MenuItem("Delete"))
            {
                scene->DestroyEntity(m_hoveredEntity);
            }
        }

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

        if (ImGui::BeginMenu("Create Lights"))
        {
            if (ImGui::MenuItem("Directional Light"))
            {
                auto entity = scene->CreateEntity("Directional Light");
                entity.AddComponent<DirectionalLightComponent>().Intensity = 1.0f;
            }

            if (ImGui::MenuItem("Point Light"))
            {
                auto entity = scene->CreateEntity("Point Light");
                entity.AddComponent<PointLightComponent>().Intensity = 1.0f;
            }

            if (ImGui::MenuItem("Spot Light"))
            {
                auto entity = scene->CreateEntity("Spot Light");
                entity.AddComponent<SpotLightComponent>().Intensity = 1.0f;
            }

            if (ImGui::MenuItem("Area Light"))
            {
                auto entity = scene->CreateEntity("Area Light");
                entity.AddComponent<AreaLightComponent>().Intensity = 1.0f;
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
