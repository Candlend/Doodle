#pragma once

#include "KeyCode.h"
#include "MouseButtonCode.h"
#include "pch.h"

namespace Doodle
{

class DOO_API Input
{
public:
    static bool IsKeyPressed(KeyCode keycode);
    static bool IsMouseButtonPressed(MouseButtonCode button);
    static std::pair<float, float> GetMousePosition();
    static float GetMouseX();
    static float GetMouseY();
};

} // namespace Doodle
