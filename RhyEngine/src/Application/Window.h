#pragma once

#include "pch.h"
#include "Core.h"
#include "Event.h"
#include <memory>

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
    using EventCallbackFn = std::function<void(Event &)>;

    static std::unique_ptr<Window> Create(const WindowProps &props = WindowProps()){
        return std::make_unique<Window>(props);
    }

    explicit Window(const WindowProps &props);
    ~Window();
    void OnUpdate();
    unsigned int GetWidth() const;
    unsigned int GetHeight() const;
    void SetVSync(bool enabled);
    bool IsVSync() const;
    void *GetNativeWindow() const;

    static Window &GetCurrentWindow();
private:
    class Impl;
    std::unique_ptr<Impl> m_impl; // 使用 PImpl
};

} // namespace RhyEngine
