#pragma once

#include "Event.h"

namespace Doodle
{

class DOO_API WindowRefreshEvent : public Event
{
public:
    WindowRefreshEvent() = default;

    EVENT_CLASS_TYPE(WindowRefresh)
};

class DOO_API WindowMoveEvent : public Event
{
public:
    WindowMoveEvent(const int x, const int y) : m_x(x), m_y(y)
    {
    }

    inline int GetX() const
    {
        return m_x;
    }
    inline int GetY() const
    {
        return m_y;
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "WindowMoveEvent: " << m_x << ", " << m_y;
        return ss.str();
    }

    EVENT_CLASS_TYPE(WindowMove)

private:
    unsigned int m_x, m_y;
};

class DOO_API WindowResizeEvent : public Event
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

private:
    unsigned int m_width, m_height;
};

class DOO_API WindowCloseEvent : public Event
{
public:
    WindowCloseEvent() = default;

    EVENT_CLASS_TYPE(WindowClose)
};

class DOO_API AppTickEvent : public Event
{
public:
    AppTickEvent() = default;

    EVENT_CLASS_TYPE(AppTick)
};

class DOO_API AppUpdateEvent : public Event
{
public:
    AppUpdateEvent() = default;

    EVENT_CLASS_TYPE(AppUpdate)
};

class DOO_API AppRenderEvent : public Event
{
public:
    AppRenderEvent() = default;

    EVENT_CLASS_TYPE(AppRender)
};

class DOO_API AppLayoutEvent : public Event
{
public:
    AppLayoutEvent() = default;

    EVENT_CLASS_TYPE(AppLayout)
};

} // namespace Doodle
