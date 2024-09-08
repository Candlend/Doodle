#include "pch.h"
#include <functional>

#include "ApplicationEvent.h"
#include "EventManager.h"
#include "Input.h"
#include "Singleton.h"

namespace Doodle
{

struct ShortcutData
{
    ShortcutData() = default;

    ShortcutData(const std::string &title, const std::string &shortcut, const std::string &category,
                 std::function<void()> action)
        : Title(title), Shortcut(shortcut), Category(category), Callback(action)
    {
    }

    std::string Title;
    std::string Shortcut;
    std::string Category;
    std::function<void()> Callback;
};

class DOO_API ShortcutManager : public Singleton<ShortcutManager>
{
public:
    ShortcutManager();

    ~ShortcutManager();

    const std::vector<ShortcutData> &GetShortcuts() const
    {
        return m_shortcuts;
    }

    void SetShortcuts(const std::vector<ShortcutData> &shortcuts)
    {
        m_shortcuts = shortcuts;
    }

    void OnUpdate();

    void RegisterShortcut(const std::string &title, const std::string &shortcut, const std::string &category,
                          std::function<void()> action);

    void UnregisterShortcut(const std::string &shortcut);

    void UnregisterAllShortcuts();

private:
    bool IsShortcutPressed(const std::string &shortcut);

    std::vector<ShortcutData> m_shortcuts;
};

} // namespace Doodle
