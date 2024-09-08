#pragma once

#include "pch.h"

#include "KeyCode.h"
#include "MouseButton.h"
#include "Singleton.h"
#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <utility>

namespace Doodle
{

struct ControllerButtonData
{
    int Button;
    KeyState State = KeyState::None;
    KeyState OldState = KeyState::None;
};

struct Controller
{
    int ID;
    std::string Name;
    std::map<int, bool> ButtonDown;
    std::map<int, ControllerButtonData> ButtonStates;
    std::map<int, float> AxisStates;
    std::map<int, float> DeadZones;
    std::map<int, uint8_t> HatStates;
};

struct KeyData
{
    KeyCode Key;
    KeyState State = KeyState::None;
    KeyState OldState = KeyState::None;
};

struct ButtonData
{
    MouseButton Button;
    KeyState State = KeyState::None;
    KeyState OldState = KeyState::None;
};

class DOO_API Input : public Singleton<Input>
{
public:
    friend class Window;
    void Initialize();
    void Deinitialize();

    static bool IsKeyPressed(KeyCode keycode);
    static bool IsKeyHeld(KeyCode keycode);
    static bool IsKeyDown(KeyCode keycode);
    static bool IsKeyReleased(KeyCode keycode);

    static bool IsMouseButtonPressed(MouseButton button);
    static bool IsMouseButtonHeld(MouseButton button);
    static bool IsMouseButtonDown(MouseButton button);
    static bool IsMouseButtonReleased(MouseButton button);
    static float GetMouseX();
    static float GetMouseY();
    static float GetMouseScrollX();
    static float GetMouseScrollY();
    static std::pair<float, float> GetMouseScroll();
    static std::pair<float, float> GetMousePosition();

    // Controllers
    static bool IsControllerPresent(int id);
    static std::vector<int> GetConnectedControllerIDs();
    static const Controller *GetController(int id);
    static std::string_view GetControllerName(int id);

    static bool IsControllerButtonPressed(int controllerID, int button);
    static bool IsControllerButtonHeld(int controllerID, int button);
    static bool IsControllerButtonDown(int controllerID, int button);
    static bool IsControllerButtonReleased(int controllerID, int button);

    static float GetControllerAxis(int controllerID, int axis);
    static uint8_t GetControllerHat(int controllerID, int hat);

    static float GetControllerDeadzone(int controllerID, int axis);
    static void SetControllerDeadzone(int controllerID, int axis, float deadzone);

    static const std::unordered_map<int, Controller> &GetControllers()
    {
        return s_Controllers;
    }

private:
    void BeginFrame();
    void EndFrame();
    void InitCallback(void *window);

    static void TransitionPressedKeys();
    static void TransitionPressedButtons();
    static void UpdateKeyState(KeyCode key, KeyState newState);
    static void UpdateButtonState(MouseButton button, KeyState newState);
    static void UpdateControllerButtonState(int controller, int button, KeyState newState);
    static void ClearReleasedKeys();

    inline static std::unordered_map<KeyCode, KeyData> s_KeyData;
    inline static std::unordered_map<MouseButton, ButtonData> s_MouseData;
    inline static std::unordered_map<int, Controller> s_Controllers;
    inline static std::pair<float, float> s_MouseScroll;
};

} // namespace Doodle
