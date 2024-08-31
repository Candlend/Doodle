#pragma once

#include "pch.h"

#include "Core.h"
#include "Event.h"
#include "Singleton.h"

namespace Doodle
{

struct WindowProps
{
    std::string Title;
    unsigned int Width;
    unsigned int Height;

    WindowProps(const std::string &title = "Doodle", unsigned int width = 1280, unsigned int height = 720)
        : Title(title), Width(width), Height(height)
    {
    }
};

class DOO_API Window
{
public:
    using EventCallbackFn = std::function<void(Event &)>;

    static std::shared_ptr<Window> Create(const WindowProps &props = WindowProps(), bool visible = true);

    ~Window() = default;
    virtual void PollEvents() = 0;
    virtual void SwapBuffers() = 0;
    virtual unsigned int GetWidth() const = 0;
    virtual unsigned int GetHeight() const = 0;
    virtual void SetVSync(bool enabled) = 0;
    virtual bool IsVSync() const = 0;
    virtual void *GetNativeWindow() const = 0;
};

} // namespace Doodle
