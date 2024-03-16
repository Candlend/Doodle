#include "EventManager.h"

namespace RhyEngine
{

void EventManager::AddListener(EventType eventType, EventCallback callback)
{
    auto &listeners = m_eventListeners[eventType];
    listeners.push_back(callback);
}

void EventManager::RemoveListener(EventType eventType, EventCallback callback)
{
    auto &listeners = m_eventListeners[eventType];
    listeners.erase(
        std::remove_if(listeners.begin(), listeners.end(),
                       [&](const EventCallback &listener) {
                           return listener.target<void(BaseEvent&)>() == callback.target<void(BaseEvent&)>(); // 这里进行比较
                       }),
        listeners.end());
}

void EventManager::Dispatch(BaseEvent &event)
{
    EventType eventType = event.GetEventType();
    auto it = m_eventListeners.find(eventType);
    if (it != m_eventListeners.end())
    {
        for (auto &listener : it->second)
        {
            listener(event);
        }
    }
}

} // namespace RhyEngine