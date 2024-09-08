#pragma once

#include "EventManager.h"

namespace Doodle
{

template <typename EventType, int ExecutionOrder = ExecutionOrder::Default> class IEventHandler
{
public:
    IEventHandler()
    {
        EventManager::Get()->AddListener<EventType>(this, &IEventHandler::OnEvent, ExecutionOrder);
    }

    virtual ~IEventHandler()
    {
        EventManager::Get()->RemoveListener<EventType>(this, &IEventHandler::OnEvent);
    }

protected:
    virtual bool OnEvent(EventType &event) = 0;
};

} // namespace Doodle