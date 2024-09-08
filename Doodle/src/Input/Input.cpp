#include "Log.h"
#include "pch.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_internal.h>

#include "ApplicationEvent.h"
#include "ApplicationRunner.h"
#include "EventManager.h"
#include "Input.h"
#include "KeyCode.h"
#include "KeyEvent.h"
#include "MouseEvent.h"
#include "Window.h"

namespace Doodle
{

void Input::Initialize()
{
    EventManager::Get()->AddListener<AppUpdateEvent>(this, &Input::BeginFrame, ExecutionOrder::First);
    EventManager::Get()->AddListener<AppRenderEvent>(this, &Input::EndFrame, ExecutionOrder::Last);
}

void Input::Deinitialize()
{
    EventManager::Get()->RemoveListener<AppUpdateEvent>(this, &Input::BeginFrame);
    EventManager::Get()->RemoveListener<AppRenderEvent>(this, &Input::EndFrame);
}

void Input::BeginFrame()
{
    static std::unordered_set<GLFWwindow *> s_InitializedWindows;
    ImGuiPlatformIO &platformIo = ImGui::GetPlatformIO();
    for (int i = 0; i < platformIo.Viewports.Size; i++)
    {
        ImGuiViewport *viewport = platformIo.Viewports[i];
        if (!viewport->PlatformUserData)
            continue;

        GLFWwindow *windowHandle =
            *static_cast<GLFWwindow **>(viewport->PlatformUserData); // First member is GLFWwindow
        if (!windowHandle)
            continue;
        // 检查窗口是否已经初始化
        if (!s_InitializedWindows.contains(windowHandle))
        {
            InitCallback(windowHandle);
            s_InitializedWindows.insert(windowHandle); // 标记为已初始化
        }
    }

    // Cleanup disconnected controller
    for (auto it = s_Controllers.begin(); it != s_Controllers.end();)
    {
        int id = it->first;
        if (glfwJoystickPresent(id) != GLFW_TRUE)
            it = s_Controllers.erase(it);
        else
            it++;
    }

    // Update controllers
    for (int id = GLFW_JOYSTICK_1; id < GLFW_JOYSTICK_LAST; id++)
    {
        if (glfwJoystickPresent(id) == GLFW_TRUE)
        {
            Controller &controller = s_Controllers[id];
            controller.ID = id;
            controller.Name = glfwGetJoystickName(id);

            int buttonCount;
            const unsigned char *buttons = glfwGetJoystickButtons(id, &buttonCount);
            for (int i = 0; i < buttonCount; i++)
            {
                if (buttons[i] == GLFW_PRESS && !controller.ButtonDown[i])
                    controller.ButtonStates[i].State = KeyState::Pressed;
                else if (buttons[i] == GLFW_RELEASE && controller.ButtonDown[i])
                    controller.ButtonStates[i].State = KeyState::Released;

                controller.ButtonDown[i] = buttons[i] == GLFW_PRESS;
            }

            int axisCount;
            const float *axes = glfwGetJoystickAxes(id, &axisCount);
            for (int i = 0; i < axisCount; i++)
                controller.AxisStates[i] = abs(axes[i]) > controller.DeadZones[i] ? axes[i] : 0.0f;

            int hatCount;
            const unsigned char *hats = glfwGetJoystickHats(id, &hatCount);
            for (int i = 0; i < hatCount; i++)
                controller.HatStates[i] = hats[i];
        }
    }

    TransitionPressedKeys();
    TransitionPressedButtons();
}

void Input::EndFrame()
{
    ClearReleasedKeys();
}

void Input::InitCallback(void *window)
{
    GLFWwindow *windowHandle = static_cast<GLFWwindow *>(window);
    static GLFWkeyfun s_PrevKeyCallback = glfwSetKeyCallback(windowHandle, nullptr);
    glfwSetKeyCallback(windowHandle, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        if (s_PrevKeyCallback)
            s_PrevKeyCallback(window, key, scancode, action, mods);
        KeyCode keycode = static_cast<KeyCode>(key);
        switch (action)
        {
        case GLFW_PRESS: {
            UpdateKeyState(keycode, KeyState::Pressed);
            EventManager::Get()->Dispatch<KeyPressedEvent>(keycode, false);
            break;
        }
        case GLFW_RELEASE: {
            UpdateKeyState(keycode, KeyState::Released);
            EventManager::Get()->Dispatch<KeyReleasedEvent>(keycode);
            break;
        }
        case GLFW_REPEAT: {
            EventManager::Get()->Dispatch<KeyPressedEvent>(keycode, true);
            break;
        }
        }
    });

    static GLFWcharfun s_PrevCharCallback = glfwSetCharCallback(windowHandle, nullptr);
    glfwSetCharCallback(windowHandle, [](GLFWwindow *window, unsigned int keycode) {
        if (s_PrevCharCallback)
            s_PrevCharCallback(window, keycode);
        EventManager::Get()->Dispatch<CharInputEvent>(keycode);
    });

    static GLFWmousebuttonfun s_PrevMouseButtonCallback = glfwSetMouseButtonCallback(windowHandle, nullptr);
    glfwSetMouseButtonCallback(windowHandle, [](GLFWwindow *window, int button, int action, int /*mods*/) {
        if (s_PrevMouseButtonCallback)
            s_PrevMouseButtonCallback(window, button, action, 0);
        MouseButton btn = static_cast<MouseButton>(button);
        switch (action)
        {
        case GLFW_PRESS: {
            UpdateButtonState(btn, KeyState::Pressed);
            EventManager::Get()->Dispatch<MouseButtonPressedEvent>(btn);
            break;
        }
        case GLFW_RELEASE: {
            UpdateButtonState(btn, KeyState::Released);
            EventManager::Get()->Dispatch<MouseButtonReleasedEvent>(btn);
            break;
        }
        }
    });

    static GLFWscrollfun s_PrevScrollCallback = glfwSetScrollCallback(windowHandle, nullptr);
    glfwSetScrollCallback(windowHandle, [](GLFWwindow *window, double xOffset, double yOffset) {
        if (s_PrevScrollCallback)
            s_PrevScrollCallback(window, xOffset, yOffset);
        EventManager::Get()->Dispatch<MouseScrolledEvent>(static_cast<float>(xOffset), static_cast<float>(yOffset));
    });

    static GLFWcursorposfun s_PrevCursorPosCallback = glfwSetCursorPosCallback(windowHandle, nullptr);
    glfwSetCursorPosCallback(windowHandle, [](GLFWwindow *window, double xPos, double yPos) {
        if (s_PrevCursorPosCallback)
            s_PrevCursorPosCallback(window, xPos, yPos);
        EventManager::Get()->Dispatch<MouseMovedEvent>(static_cast<float>(xPos), static_cast<float>(yPos));
    });

    static GLFWjoystickfun s_PrevJoystickCallback = glfwSetJoystickCallback(nullptr);
    glfwSetJoystickCallback([](int id, int event) {
        if (s_PrevJoystickCallback)
            s_PrevJoystickCallback(id, event);
        if (event == GLFW_CONNECTED)
        {
            Controller &controller = s_Controllers[id];
            controller.ID = id;
            controller.Name = glfwGetJoystickName(id);
        }
        else if (event == GLFW_DISCONNECTED)
        {
            s_Controllers.erase(id);
        }
    });
}

bool Input::IsKeyPressed(KeyCode key)
{
    return s_KeyData.contains(key) && s_KeyData[key].State == KeyState::Pressed;
}

bool Input::IsKeyHeld(KeyCode key)
{
    return s_KeyData.contains(key) && s_KeyData[key].State == KeyState::Held;
}

bool Input::IsKeyDown(KeyCode keycode)
{
    return s_KeyData.find(keycode) != s_KeyData.end() && s_KeyData[keycode].State != KeyState::None;
}

bool Input::IsKeyReleased(KeyCode key)
{
    return s_KeyData.contains(key) && s_KeyData[key].State == KeyState::Released;
}

bool Input::IsMouseButtonPressed(MouseButton button)
{
    return s_MouseData.contains(button) && s_MouseData[button].State == KeyState::Pressed;
}

bool Input::IsMouseButtonHeld(MouseButton button)
{
    return s_MouseData.contains(button) && s_MouseData[button].State == KeyState::Held;
}

bool Input::IsMouseButtonDown(MouseButton button)
{
    return s_MouseData.contains(button) && s_MouseData[button].State != KeyState::None;
}

bool Input::IsMouseButtonReleased(MouseButton button)
{
    return s_MouseData.contains(button) && s_MouseData[button].State == KeyState::Released;
}

float Input::GetMouseX()
{
    auto [x, y] = GetMousePosition();
    return static_cast<float>(x);
}

float Input::GetMouseY()
{
    auto [x, y] = GetMousePosition();
    return static_cast<float>(y);
}

float Input::GetMouseScrollX()
{
    auto [x, y] = GetMouseScroll();
    return static_cast<float>(x);
}

float Input::GetMouseScrollY()
{
    auto [x, y] = GetMouseScroll();
    return static_cast<float>(y);
}

std::pair<float, float> Input::GetMouseScroll()
{
    return s_MouseScroll;
}

std::pair<float, float> Input::GetMousePosition()
{
    // TODO 区分Editor和Game窗口
    auto window = ApplicationRunner().GetWindow();
    GLFWwindow *windowHandle = static_cast<GLFWwindow *>(window->GetNativeWindow());
    double x, y;
    glfwGetCursorPos(windowHandle, &x, &y);
    return {static_cast<float>(x), static_cast<float>(y)};
}

bool Input::IsControllerPresent(int id)
{
    return s_Controllers.find(id) != s_Controllers.end();
}

std::vector<int> Input::GetConnectedControllerIDs()
{
    std::vector<int> ids;
    ids.reserve(s_Controllers.size());
    for (const auto &[id, controller] : s_Controllers)
        ids.emplace_back(id);

    return ids;
}

const Controller *Input::GetController(int id)
{
    if (!Input::IsControllerPresent(id))
        return nullptr;

    return &s_Controllers.at(id);
}

std::string_view Input::GetControllerName(int id)
{
    if (!Input::IsControllerPresent(id))
        return {};

    return s_Controllers.at(id).Name;
}

bool Input::IsControllerButtonPressed(int controllerID, int button)
{
    if (!Input::IsControllerPresent(controllerID))
        return false;

    auto &contoller = s_Controllers.at(controllerID);
    return contoller.ButtonStates.contains(button) && contoller.ButtonStates[button].State == KeyState::Pressed;
}

bool Input::IsControllerButtonHeld(int controllerID, int button)
{
    if (!Input::IsControllerPresent(controllerID))
        return false;

    auto &contoller = s_Controllers.at(controllerID);
    return contoller.ButtonStates.contains(button) && contoller.ButtonStates[button].State == KeyState::Held;
}

bool Input::IsControllerButtonDown(int controllerID, int button)
{
    if (!Input::IsControllerPresent(controllerID))
        return false;

    const Controller &controller = s_Controllers.at(controllerID);
    if (controller.ButtonDown.find(button) == controller.ButtonDown.end())
        return false;

    return controller.ButtonDown.at(button);
}

bool Input::IsControllerButtonReleased(int controllerID, int button)
{
    if (!Input::IsControllerPresent(controllerID))
        return true;

    auto &contoller = s_Controllers.at(controllerID);
    return contoller.ButtonStates.contains(button) && contoller.ButtonStates[button].State == KeyState::Released;
}

float Input::GetControllerAxis(int controllerID, int axis)
{
    if (!Input::IsControllerPresent(controllerID))
        return 0.0f;

    const Controller &controller = s_Controllers.at(controllerID);
    if (controller.AxisStates.find(axis) == controller.AxisStates.end())
        return 0.0f;

    return controller.AxisStates.at(axis);
}

uint8_t Input::GetControllerHat(int controllerID, int hat)
{
    if (!Input::IsControllerPresent(controllerID))
        return 0;

    const Controller &controller = s_Controllers.at(controllerID);
    if (controller.HatStates.find(hat) == controller.HatStates.end())
        return 0;

    return controller.HatStates.at(hat);
}

float Input::GetControllerDeadzone(int controllerID, int axis)
{
    if (!Input::IsControllerPresent(controllerID))
        return 0.0f;

    const Controller &controller = s_Controllers.at(controllerID);
    return controller.DeadZones.at(axis);
}

void Input::SetControllerDeadzone(int controllerID, int axis, float deadzone)
{
    if (!Input::IsControllerPresent(controllerID))
        return;

    Controller &controller = s_Controllers.at(controllerID);
    controller.DeadZones[axis] = deadzone;
}

void Input::TransitionPressedKeys()
{
    for (const auto &[key, keyData] : s_KeyData)
    {
        if (keyData.State == KeyState::Pressed)
            UpdateKeyState(key, KeyState::Held);
    }
}

void Input::TransitionPressedButtons()
{
    for (const auto &[button, buttonData] : s_MouseData)
    {
        if (buttonData.State == KeyState::Pressed)
            UpdateButtonState(button, KeyState::Held);
    }

    for (const auto &[id, controller] : s_Controllers)
    {
        for (const auto &[button, buttonStates] : controller.ButtonStates)
        {
            if (buttonStates.State == KeyState::Pressed)
                UpdateControllerButtonState(id, button, KeyState::Held);
        }
    }
}

void Input::UpdateKeyState(KeyCode key, KeyState newState)
{
    auto &keyData = s_KeyData[key];
    keyData.Key = key;
    keyData.OldState = keyData.State;
    keyData.State = newState;
}

void Input::UpdateButtonState(MouseButton button, KeyState newState)
{
    auto &mouseData = s_MouseData[button];
    mouseData.Button = button;
    mouseData.OldState = mouseData.State;
    mouseData.State = newState;
}

void Input::UpdateControllerButtonState(int controllerID, int button, KeyState newState)
{
    auto &controllerButtonData = s_Controllers.at(controllerID).ButtonStates.at(button);
    controllerButtonData.Button = button;
    controllerButtonData.OldState = controllerButtonData.State;
    controllerButtonData.State = newState;
}

void Input::ClearReleasedKeys()
{
    for (const auto &[key, keyData] : s_KeyData)
    {
        if (keyData.State == KeyState::Released)
            UpdateKeyState(key, KeyState::None);
    }

    for (const auto &[button, buttonData] : s_MouseData)
    {
        if (buttonData.State == KeyState::Released)
            UpdateButtonState(button, KeyState::None);
    }

    for (const auto &[id, controller] : s_Controllers)
    {
        for (const auto &[button, buttonStates] : controller.ButtonStates)
        {
            if (buttonStates.State == KeyState::Released)
                UpdateControllerButtonState(id, button, KeyState::None);
        }
    }
}

} // namespace Doodle
