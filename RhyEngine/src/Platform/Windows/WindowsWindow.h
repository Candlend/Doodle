#pragma once

#include <GLFW/glfw3.h>

#include "RhyEngine/Window.h"

namespace RhyEngine
{

class WindowsWindow : public Window
{
public:
    explicit WindowsWindow(const WindowProps &props);
    virtual ~WindowsWindow();

    void OnUpdate() override;

    inline unsigned int GetWidth() const override
    {
        return m_data.Width;
    }
    inline unsigned int GetHeight() const override
    {
        return m_data.Height;
    }

    void SetEventCallback(const EventCallback &callback) override
    {
        m_data.EventCallback = callback;
    }
    void SetVSync(bool enabled) override;
    bool IsVSync() const override;

private:
    struct WindowData
    {
        std::string Title;
        unsigned int Width, Height;
        bool VSync;

        EventCallback EventCallback;
        std::unordered_map<int, int> KeyRepeatCounts;
    };

    virtual void Init(const WindowProps &props);
    virtual void Shutdown();

    GLFWwindow *m_window;

    WindowData m_data;
};

} // namespace RhyEngine
