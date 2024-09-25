#pragma once

#include "pch.h"

namespace Doodle
{

#define EVENT_CLASS_TYPE(type)                                                                                         \
    static std::string GetStaticType()                                                                                 \
    {                                                                                                                  \
        return #type;                                                                                                  \
    }                                                                                                                  \
    virtual std::string GetEventType() const override                                                                  \
    {                                                                                                                  \
        return GetStaticType();                                                                                        \
    }

class EventManager;

class DOO_API Event
{
    friend class EventManager;

public:
    bool Handled = false;

    virtual std::string GetEventType() const = 0;
    virtual std::string ToString() const
    {
        return GetEventType();
    }
};

inline std::ostream &operator<<(std::ostream &os, const Event &e)
{
    return os << e.ToString();
}

} // namespace Doodle
