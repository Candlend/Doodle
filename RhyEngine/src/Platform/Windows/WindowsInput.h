#pragma once

#include "pch.h"

#include "Input.h"

namespace RhyEngine
{

class WindowsInput : public Input
{
protected:
    virtual bool IsKeyPressedImpl(KeyCode keycode) override;

    virtual bool IsMouseButtonPressedImpl(MouseButtonCode button) override;
    virtual std::pair<float, float> GetMousePositionImpl() override;
    virtual float GetMouseXImpl() override;
    virtual float GetMouseYImpl() override;
};

} // namespace RhyEngine
