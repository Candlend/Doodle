#include <imgui.h>

#include "ImGuiMenu.h"
#include "ImGuiUtils.h"

namespace Doodle
{

void ImGuiMenu::RegisterShortcuts()
{
    for (const auto &menuItem : m_menuItems)
    {
        RegisterShortcut(menuItem, menuItem->Title);
    }
}

void ImGuiMenu::RegisterShortcut(std::shared_ptr<ImGuiMenuItem> item, const std::string &category)
{
    if (auto *parentMenuItem = dynamic_cast<ParentMenuItem *>(item.get()))
    {
        for (const auto &subItem : parentMenuItem->SubItems)
        {
            RegisterShortcut(subItem, category);
        }
    }
    else if (auto *selectableMenuItem = dynamic_cast<SelectableMenuItem *>(item.get()))
    {
        ShortcutManager::Get()->RegisterShortcut(selectableMenuItem->Title, selectableMenuItem->Shortcut, category,
                                                 [=]() { selectableMenuItem->Toggle(); });
    }
    else if (auto *clickableMenuItem = dynamic_cast<ClickableMenuItem *>(item.get()))
    {
        ShortcutManager::Get()->RegisterShortcut(clickableMenuItem->Title, clickableMenuItem->Shortcut, category,
                                                 [=]() { clickableMenuItem->OnClick(); });
    }
}

void ImGuiMenu::OnLayout()
{
    ImGuiUtils::MenuBarScope menuBarScope;
    for (const auto &menuItem : m_menuItems)
    {
        DrawMenuItem(menuItem);
    }
}

void ImGuiMenu::DrawMenuItem(std::shared_ptr<ImGuiMenuItem> item)
{
    if (auto *parentMenuItem = dynamic_cast<ParentMenuItem *>(item.get()))
    {
        if (ImGui::BeginMenu(parentMenuItem->Title.c_str(), parentMenuItem->Enabled))
        {
            for (const auto &subItem : parentMenuItem->SubItems)
            {
                DrawMenuItem(subItem);
            }
            ImGui::EndMenu();
        }
    }
    else if (auto *childMenuItem = dynamic_cast<ChildMenuItem *>(item.get()))
    {
        if (auto *selectableMenuItem = dynamic_cast<SelectableMenuItem *>(item.get()))
        {
            if (selectableMenuItem->IsSelected)
            {
                selectableMenuItem->Selected = selectableMenuItem->IsSelected();
            }
            if (ImGui::MenuItem(selectableMenuItem->Title.c_str(), selectableMenuItem->Shortcut.c_str(),
                                selectableMenuItem->Selected, selectableMenuItem->Enabled))
            {
                selectableMenuItem->Toggle();
            }
        }
        else if (auto *clickableMenuItem = dynamic_cast<ClickableMenuItem *>(item.get()))
        {
            if (ImGui::MenuItem(clickableMenuItem->Title.c_str(), clickableMenuItem->Shortcut.c_str(), false,
                                clickableMenuItem->Enabled))
            {
                clickableMenuItem->OnClick();
            }
        }
    }
    else if (auto *separatorMenuItem = dynamic_cast<SeparatorMenuItem *>(item.get()))
    {
        if (!separatorMenuItem->Title.empty())
        {
            ImGui::SeparatorText(separatorMenuItem->Title.c_str());
        }
        else
        {
            ImGui::Separator();
        }
    }
}

} // namespace Doodle
