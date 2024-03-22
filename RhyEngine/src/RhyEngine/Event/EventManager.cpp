#include "EventManager.h"
#include "RhyEngine/Event/Event.h"

namespace RhyEngine
{

void EventManager::Dispatch(BaseEvent &event)
{
    EventType eventType = event.GetEventType();
    auto it = m_eventListeners.find(eventType);
    if (it != m_eventListeners.end())
    {
        for (auto &listener : it->second)
        {
            listener.Callback(event);
        }
    }
}

} // namespace RhyEngine