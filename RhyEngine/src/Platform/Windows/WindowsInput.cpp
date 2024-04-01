#ifdef RHY_PLATFORM_WINDOWS

#include "pch.h"
#include <GLFW/glfw3.h>
#include "Application.h"
#include "Input.h"
#include "KeyCode.h"

namespace RhyEngine
{

class WindowsInput : public Input
{
private:
    GLFWwindow* GetNativeWindow() const
    {
        return static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    }

protected:
    virtual bool IsKeyPressedImpl(KeyCode keycode) override
    {
        auto *window = GetNativeWindow();
        auto state = glfwGetKey(window, keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    virtual bool IsMouseButtonPressedImpl(MouseButtonCode button) override
    {
        auto *window = GetNativeWindow();
        auto state = glfwGetMouseButton(window, button);
        return state == GLFW_PRESS;
    }

    virtual std::pair<float, float> GetMousePositionImpl() override
    {
        auto *window = GetNativeWindow();
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        return {static_cast<float>(xpos), static_cast<float>(ypos)};
    }

    virtual float GetMouseXImpl() override
    {
        return GetMousePositionImpl().first;
    }

    virtual float GetMouseYImpl() override
    {
        return GetMousePositionImpl().second;
    }
};

// 静态实例化
std::unique_ptr<Input> Input::s_Instance = std::make_unique<WindowsInput>();

} // namespace RhyEngine

#endif
