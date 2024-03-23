#pragma once

#include "pch.h"

#include "Core.h"
#include "Event/Event.h"

namespace RhyEngine
{

struct WindowProps
{
    std::string Title;
    unsigned int Width;
    unsigned int Height;

    explicit WindowProps(const std::string &title = "RhyEngine", unsigned int width = 1280, unsigned int height = 720)
        : Title(title), Width(width), Height(height)
    {
    }
};

class RHY_API Window
{
public:
    using EventCallback = std::function<void(BaseEvent &)>;

    virtual ~Window() = default;

    virtual void OnUpdate() = 0;

    virtual unsigned int GetWidth() const = 0;
    virtual unsigned int GetHeight() const = 0;

    virtual void SetEventCallback(const EventCallback &callback) = 0;
    virtual void SetVSync(bool enabled) = 0;
    virtual bool IsVSync() const = 0;
    virtual void *GetNativeWindow() const = 0;
    static Window *Create(const WindowProps &props = WindowProps());
};
} // namespace RhyEngine
