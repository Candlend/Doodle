#pragma once

#include "pch.h"

#include "ShortcutManager.h"

namespace Doodle
{

struct ImGuiMenuItem
{
    ImGuiMenuItem(const std::string &title) : Title(title)
    {
    }

    virtual ~ImGuiMenuItem() = default;

    std::string Title;
    bool Enabled = true;
};

struct SeparatorMenuItem : public ImGuiMenuItem
{
};

struct ChildMenuItem : public ImGuiMenuItem
{
    ChildMenuItem(const std::string &title, const std::string &shortcut = "") : ImGuiMenuItem(title), Shortcut(shortcut)
    {
    }

    std::string Shortcut;
};

struct SelectableMenuItem : public ChildMenuItem
{
    SelectableMenuItem(const std::string &title, std::function<void(bool)> callback, const std::string &shortcut = "",
                       std::function<bool()> isSelected = nullptr)
        : ChildMenuItem(title, shortcut), OnToggle(callback), Selected(true), IsSelected(isSelected)
    {
    }

    void Toggle()
    {
        Selected = !Selected;
        OnToggle(Selected);
    }

    bool Selected = true;
    std::function<void(bool)> OnToggle;
    std::function<bool()> IsSelected;
};

struct ClickableMenuItem : public ChildMenuItem
{
    ClickableMenuItem(const std::string &title, std::function<void()> callback, const std::string &shortcut = "")
        : ChildMenuItem(title, shortcut), OnClick(callback)
    {
    }

    std::function<void()> OnClick;
};

struct ParentMenuItem : public ImGuiMenuItem
{
    ParentMenuItem(const std::string &title) : ImGuiMenuItem(title)
    {
    }

    ParentMenuItem(const std::string &title, std::initializer_list<std::shared_ptr<ImGuiMenuItem>> subItems)
        : ImGuiMenuItem(title)
    {
        for (const auto &item : subItems)
        {
            SubItems.push_back(item);
        }
    }

    template <typename T, typename... Args> void AddSubItem(Args &&...args)
    {
        static_assert(std::is_base_of<ImGuiMenuItem, T>::value, "T must be derived from ImGuiMenuItem");
        auto item = std::make_shared<T>(std::forward<Args>(args)...);
        SubItems.push_back(item);
    }

    template <typename T> void AddSubItem(std::shared_ptr<T> item)
    {
        static_assert(std::is_base_of<ImGuiMenuItem, T>::value, "T must be derived from ImGuiMenuItem");
        SubItems.push_back(item);
    }

    std::vector<std::shared_ptr<ImGuiMenuItem>> SubItems;
};

class DOO_API ImGuiMenu
{
public:
    ImGuiMenu() = default;
    ImGuiMenu(std::initializer_list<std::shared_ptr<ImGuiMenuItem>> menuItems)
    {
        for (const auto &item : menuItems)
        {
            m_menuItems.push_back(item);
        }
    }

    const std::vector<std::shared_ptr<ImGuiMenuItem>> &GetMenuItems() const
    {
        return m_menuItems;
    }

    template <typename T, typename... Args> void AddMenuItem(Args &&...args)
    {
        static_assert(std::is_base_of<ImGuiMenuItem, T>::value, "T must be derived from ImGuiMenuItem");
        auto item = std::make_shared<T>(std::forward<Args>(args)...);
        m_menuItems.push_back(item);
    }

    template <typename T> void AddMenuItem(std::shared_ptr<T> item)
    {
        static_assert(std::is_base_of<ImGuiMenuItem, T>::value, "T must be derived from ImGuiMenuItem");
        m_menuItems.push_back(item);
    }

    void RegisterShortcuts();

    void RegisterShortcut(std::shared_ptr<ImGuiMenuItem> item, const std::string &category);

    void OnLayout();

private:
    void DrawMenuItem(std::shared_ptr<ImGuiMenuItem> item);

    std::vector<std::shared_ptr<ImGuiMenuItem>> m_menuItems;
};

} // namespace Doodle
