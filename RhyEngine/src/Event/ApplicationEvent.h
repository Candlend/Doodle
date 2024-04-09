#pragma once

#include "Event.h"

namespace RhyEngine
{
class RHY_API WindowResizeEvent : public Event
{
public:
    WindowResizeEvent(const unsigned int width, const unsigned int height) : m_width(width), m_height(height)
    {
    }

    inline unsigned int GetWidth() const
    {
        return m_width;
    }
    inline unsigned int GetHeight() const
    {
        return m_height;
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "WindowResizeEvent: " << m_width << ", " << m_height;
        return ss.str();
    }

    EVENT_CLASS_TYPE(WindowResize)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)

private:
    unsigned int m_width, m_height;
};

class RHY_API WindowCloseEvent : public Event
{
public:
    WindowCloseEvent() = default;

    EVENT_CLASS_TYPE(WindowClose)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class RHY_API AppTickEvent : public Event
{
public:
    AppTickEvent() = default;

    EVENT_CLASS_TYPE(AppTick)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class RHY_API AppUpdateEvent : public Event
{
public:
    AppUpdateEvent() = default;

    EVENT_CLASS_TYPE(AppUpdate)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class RHY_API AppRenderEvent : public Event
{
public:
    AppRenderEvent() = default;

    EVENT_CLASS_TYPE(AppRender)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class RHY_API AppLayoutEvent : public Event
{
public:
    AppLayoutEvent() = default;

    EVENT_CLASS_TYPE(AppLayout)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};
} // namespace RhyEngine
