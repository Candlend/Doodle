#pragma once

#include "pch.h"
#include "RhyEngine/Core.h"
#include "EventType.h"


namespace RhyEngine
{

class EventManager;

class RHY_API BaseEvent
{
    friend class EventManager;

public:
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

protected:
    bool m_handled = false;
};

inline std::ostream &operator<<(std::ostream &os, const BaseEvent &e)
{
    return os << e.ToString();
}

} // namespace RhyEngine
