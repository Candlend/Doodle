#pragma once

#include "pch.h"

#include "Core.h"

namespace Doodle
{

enum class EventType
{
    None = 0,
    WindowClose,
    WindowRefresh,
    WindowMove,
    WindowResize,
    WindowFocus,
    WindowLostFocus,
    WindowMoved,
    AppTick,
    AppUpdate,
    AppRender,
    AppLayout,
    SceneActivate,
    SceneDeactivate,
    SelectionChanged,
    ViewportResize,
    KeyPressed,
    KeyReleased,
    CharInput,
    MouseButtonPressed,
    MouseButtonReleased,
    MouseMoved,
    MouseScrolled
};

enum EventCategory
{
    None = 0,
    EventCategoryApplication = 1 << 0,
    EventCategoryInput = 1 << 1,
    EventCategoryKeyboard = 1 << 2,
    EventCategoryMouse = 1 << 3,
    EventCategoryMouseButton = 1 << 4,
    EventCategoryScene = 1 << 5,
};

#define EVENT_CLASS_TYPE(type)                                                                                         \
    static EventType GetStaticType()                                                                                   \
    {                                                                                                                  \
        return EventType::type;                                                                                        \
    }                                                                                                                  \
    virtual EventType GetEventType() const override                                                                    \
    {                                                                                                                  \
        return GetStaticType();                                                                                        \
    }                                                                                                                  \
    virtual const char *GetName() const override                                                                       \
    {                                                                                                                  \
        return #type;                                                                                                  \
    }

#define EVENT_CLASS_CATEGORY(category)                                                                                 \
    virtual int GetCategoryFlags() const override                                                                      \
    {                                                                                                                  \
        return category;                                                                                               \
    }

class EventManager;

class DOO_API Event
{
    friend class EventManager;

public:
    bool Handled = false;

    virtual EventType GetEventType() const = 0;
    virtual const char *GetName() const = 0;
    virtual int GetCategoryFlags() const = 0;
    virtual std::string ToString() const
    {
        return GetName();
    }

    inline bool IsInCategory(EventCategory category) const
    {
        return GetCategoryFlags() & category;
    }
};

inline std::ostream &operator<<(std::ostream &os, const Event &e)
{
    return os << e.ToString();
}

} // namespace Doodle
