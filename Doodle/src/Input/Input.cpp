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
    EventManager::Get()->AddListener<AppUpdateEvent>(this, &Input::Update, ExecutionOrder::First);
}

void Input::Deinitialize()
{
    EventManager::Get()->RemoveListener<AppUpdateEvent>(this, &Input::Update);
}

void Input::Update()
{
    static std::unordered_set<GLFWwindow *> s_InitializedWindows;
    ImGuiPlatformIO &platformIo = ImGui::GetPlatformIO();
    for (int i = 0; i < platformIo.Viewports.Size; i++)
    {
        ImGuiViewport *viewport = platformIo.Viewports[i];
        if (!viewport->PlatformWindowCreated)
        {
            if (!viewport->PlatformUserData)
                continue;

            GLFWwindow *windowHandle =
                *static_cast<GLFWwindow **>(viewport->PlatformUserData); // First member is GLFWwindow
            if (!windowHandle)
                continue;
            // 检查窗口是否已经初始化
            if (s_InitializedWindows.contains(windowHandle))
            {
                InitCallback(windowHandle);
                s_InitializedWindows.insert(windowHandle); // 标记为已初始化
            }
        }
    }
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
            EventManager::Get()->Dispatch<KeyPressedEvent>(keycode, false);
            break;
        }
        case GLFW_RELEASE: {
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
        MouseButtonCode btn = static_cast<MouseButtonCode>(button);
        switch (action)
        {
        case GLFW_PRESS: {
            EventManager::Get()->Dispatch<MouseButtonPressedEvent>(btn);
            break;
        }
        case GLFW_RELEASE: {
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
}

bool Input::IsKeyPressed(KeyCode keycode)
{
    auto window = ApplicationRunner().GetWindow();
    GLFWwindow *win = static_cast<GLFWwindow *>(window->GetNativeWindow());
    ImGuiContext *context = ImGui::GetCurrentContext();
    bool pressed = false;
    for (ImGuiViewport *viewport : context->Viewports)
    {
        if (!viewport->PlatformUserData)
            continue;

        GLFWwindow *windowHandle =
            *static_cast<GLFWwindow **>(viewport->PlatformUserData); // First member is GLFWwindow
        if (!windowHandle)
            continue;
        auto state = glfwGetKey(windowHandle, static_cast<int32_t>(keycode));
        if (state == GLFW_PRESS || state == GLFW_REPEAT)
        {
            pressed = true;
            break;
        }
    }

    return pressed;
}

bool Input::IsMouseButtonPressed(MouseButtonCode button)
{
    auto window = ApplicationRunner().GetWindow();
    GLFWwindow *win = static_cast<GLFWwindow *>(window->GetNativeWindow());
    ImGuiContext *context = ImGui::GetCurrentContext();
    bool pressed = false;
    for (ImGuiViewport *viewport : context->Viewports)
    {
        if (!viewport->PlatformUserData)
            continue;

        GLFWwindow *windowHandle =
            *static_cast<GLFWwindow **>(viewport->PlatformUserData); // First member is GLFWwindow
        if (!windowHandle)
            continue;
        auto state = glfwGetMouseButton(windowHandle, static_cast<int32_t>(button));
        if (state == GLFW_PRESS)
        {
            pressed = true;
            break;
        }
    }

    return pressed;
}

std::pair<float, float> Input::GetMousePosition()
{
    auto window = ApplicationRunner().GetWindow();
    GLFWwindow *win = static_cast<GLFWwindow *>(window->GetNativeWindow());
    ImGuiContext *context = ImGui::GetCurrentContext();
    std::pair<float, float> pos = {0.0f, 0.0f};
    for (ImGuiViewport *viewport : context->Viewports)
    {
        if (!viewport->PlatformUserData)
            continue;

        GLFWwindow *windowHandle =
            *static_cast<GLFWwindow **>(viewport->PlatformUserData); // First member is GLFWwindow
        if (!windowHandle)
            continue;
        double x, y;
        glfwGetCursorPos(windowHandle, &x, &y);
        pos.first = static_cast<float>(x);
        pos.second = static_cast<float>(y);
        break;
    }

    return pos;
}

float Input::GetMouseX()
{
    return GetMousePosition().first;
}

float Input::GetMouseY()
{
    return GetMousePosition().second;
}

} // namespace Doodle
