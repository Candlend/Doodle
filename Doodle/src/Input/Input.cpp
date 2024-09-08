#include "Input.h"
#include "ApplicationRunner.h"
#include "KeyCode.h"
#include "Window.h"
#include "imgui.h"
#include "pch.h"
#include <GLFW/glfw3.h>

namespace Doodle
{

bool Input::IsKeyPressed(KeyCode keycode)
{
    return ImGui::IsKeyDown(static_cast<ImGuiKey>(keycode));
}

bool Input::IsMouseButtonPressed(MouseButtonCode button)
{
    return ImGui::IsMouseDown(button);
}

std::pair<float, float> Input::GetMousePosition()
{
    ImVec2 pos = ImGui::GetMousePos();
    return {pos.x, pos.y};
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
