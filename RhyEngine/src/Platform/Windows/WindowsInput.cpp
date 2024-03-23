#include "RhyEngine/KeyCode.h"
#include "pch.h"
#include <GLFW/glfw3.h>

#include "RhyEngine/Application.h"
#include "WindowsInput.h"


namespace RhyEngine
{

Input *Input::s_Instance = new WindowsInput();

bool WindowsInput::IsKeyPressedImpl(KeyCode keycode)
{
    auto *window = static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetKey(window, keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool WindowsInput::IsMouseButtonPressedImpl(MouseButtonCode button)
{
    auto *window = static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetMouseButton(window, button);
    return state == GLFW_PRESS;
}

std::pair<float, float> WindowsInput::GetMousePositionImpl()
{
    auto *window = static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow());
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    return {static_cast<float>(xpos), static_cast<float>(ypos)};
}

float WindowsInput::GetMouseXImpl()
{
    auto [x, y] = GetMousePositionImpl();
    return x;
}

float WindowsInput::GetMouseYImpl()
{
    auto [x, y] = GetMousePositionImpl();
    return y;
}

} // namespace RhyEngine
