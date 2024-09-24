#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <magic_enum.hpp>
#include <rfl.hpp>

#include "ApplicationEvent.h"
#include "EventManager.h"
#include "Input.h"
#include "KeyCode.h"
#include "ShortcutManager.h"

namespace Doodle
{

ShortcutManager::ShortcutManager()
{
    EventManager::Get()->AddListener<AppUpdateEvent>(this, &ShortcutManager::OnUpdate, ExecutionOrder::Early);
}

ShortcutManager::~ShortcutManager()
{
    EventManager::Get()->RemoveListener<AppUpdateEvent>(this, &ShortcutManager::OnUpdate);
}

void ShortcutManager::OnUpdate()
{
    for (const auto &shortcutData : m_shortcuts)
    {
        if (IsShortcutPressed(shortcutData.Shortcut))
        {
            shortcutData.Callback();
        }
    }
}

void ShortcutManager::RegisterShortcut(const std::string &title, const std::string &shortcut,
                                       const std::string &category, std::function<void()> action)
{
    if (shortcut.empty())
        return;
    ShortcutData shortcutData(title, shortcut, category, action);
    m_shortcuts.push_back(shortcutData);
}

void ShortcutManager::UnregisterShortcut(const std::string &shortcut)
{
    m_shortcuts.erase(std::remove_if(m_shortcuts.begin(), m_shortcuts.end(),
                                     [shortcut](const ShortcutData &data) { return data.Shortcut == shortcut; }),
                      m_shortcuts.end());
}

void ShortcutManager::UnregisterAllShortcuts()
{
    m_shortcuts.clear();
}

bool ShortcutManager::IsShortcutPressed(const std::string &shortcut)
{
    std::vector<std::string> keys;

    boost::split(keys, shortcut, boost::is_any_of("+"));
    bool shiftPressed = Input::IsKeyDown(KeyCode::LeftShift) || Input::IsKeyDown(KeyCode::RightShift);
    bool ctrlPressed = Input::IsKeyDown(KeyCode::LeftControl) || Input::IsKeyDown(KeyCode::RightControl);
    bool altPressed = Input::IsKeyDown(KeyCode::LeftAlt) || Input::IsKeyDown(KeyCode::RightAlt);

    bool shiftRequired = false;
    bool ctrlRequired = false;
    bool altRequired = false;

    KeyCode mainKey = magic_enum::enum_cast<KeyCode>(keys.back()).value();

    bool mainKeyPressed = Input::IsKeyPressed(mainKey);

    for (const auto &key : keys)
    {
        if (key == "Ctrl")
            ctrlRequired = true;
        else if (key == "Shift")
            shiftRequired = true;
        else if (key == "Alt")
            altRequired = true;
    }

    bool shiftMatch = shiftRequired == shiftPressed;
    bool ctrlMatch = ctrlRequired == ctrlPressed;
    bool altMatch = altRequired == altPressed;

    return shiftMatch && ctrlMatch && altMatch && mainKeyPressed;
}

} // namespace Doodle
