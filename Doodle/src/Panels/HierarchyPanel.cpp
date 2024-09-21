#include "HierarchyPanel.h"
#include "Component.h"
#include "EditorCamera.h"
#include "Entity.h"
#include "PanelManager.h"
#include "SceneManager.h"
#include "SelectionManager.h"
#include "imgui.h"

namespace Doodle
{

void HierarchyPanel::RenderEntityTree(std::vector<Entity> &entities)
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

        if (ImGui::TreeNodeEx(tag.c_str(), flags))
        {
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

            // Render the children of the entity recursively
            ImGui::Indent(16);
            RenderEntityTree(children);
            ImGui::Unindent(16);
            // Close the tree node after rendering children
            ImGui::TreePop();
        }
    }
}
void HierarchyPanel::OnPanelLayout()
{
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
    RenderEntityTree(rootEntities);

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
