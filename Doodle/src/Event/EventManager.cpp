#include "EventManager.h"
#include "Event.h"

namespace Doodle
{

void EventManager::Dispatch(Event &event)
{
    EventType eventType = event.GetEventType();
    auto it = m_eventListeners.find(eventType);
    if (it != m_eventListeners.end())
    {
        for (int i = 0; i < it->second.size(); i++)
        {
            auto listener = it->second[i];
            if (listener.Callback(event))
            {
                break;
            }
        }
    }
}


} // namespace Doodle