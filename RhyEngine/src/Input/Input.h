#pragma once

#include "pch.h"

#include "Core.h"
#include "KeyCode.h"
#include "MouseButtonCode.h"

namespace RhyEngine
{

class RHY_API Input
{
public:
    inline static bool IsKeyPressed(KeyCode keycode)
    {
        return s_Instance->IsKeyPressedImpl(keycode);
    }

    inline static bool IsMouseButtonPressed(MouseButtonCode button)
    {
        return s_Instance->IsMouseButtonPressedImpl(button);
    }
    inline static std::pair<float, float> GetMousePosition()
    {
        return s_Instance->GetMousePositionImpl();
    }
    inline static float GetMouseX()
    {
        return s_Instance->GetMouseXImpl();
    }
    inline static float GetMouseY()
    {
        return s_Instance->GetMouseYImpl();
    }

protected:
    virtual bool IsKeyPressedImpl(KeyCode keycode) = 0;

    virtual bool IsMouseButtonPressedImpl(MouseButtonCode button) = 0;
    virtual std::pair<float, float> GetMousePositionImpl() = 0;
    virtual float GetMouseXImpl() = 0;
    virtual float GetMouseYImpl() = 0;

private:
    static Input *s_Instance;
};

} // namespace RhyEngine