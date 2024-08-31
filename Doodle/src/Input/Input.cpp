#include "Input.h"
#include "ApplicationRunner.h"
#include "KeyCode.h"
#include "Window.h"
#include "pch.h"
#include <GLFW/glfw3.h>

namespace Doodle
{

bool Input::IsKeyPressed(KeyCode keycode)
{
    auto *window = static_cast<GLFWwindow *>(ApplicationRunner::GetWindow()->GetNativeWindow());
    auto state = glfwGetKey(window, keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::IsMouseButtonPressed(MouseButtonCode button)
{
    auto *window = static_cast<GLFWwindow *>(ApplicationRunner::GetWindow()->GetNativeWindow());
    auto state = glfwGetMouseButton(window, button);
    return state == GLFW_PRESS;
}

std::pair<float, float> Input::GetMousePosition()
{
    auto *window = static_cast<GLFWwindow *>(ApplicationRunner::GetWindow()->GetNativeWindow());
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return {static_cast<float>(xpos), static_cast<float>(ypos)};
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
