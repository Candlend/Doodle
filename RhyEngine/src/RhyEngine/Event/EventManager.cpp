#include "EventManager.h"

namespace RhyEngine
{

template <typename T> void EventManager::AddListener(EventType eventType, EventCallback callback)
{
    auto &listeners = m_eventListeners[eventType];
    listeners.push_back(callback);
}

template <typename T> void EventManager::RemoveListener(EventType eventType, EventCallback callback)
{
    auto &listeners = m_eventListeners[eventType];
    listeners.erase(
        std::remove_if(listeners.begin(), listeners.end(),
                       [&](const EventCallback &listener) { return listener.target<T>() == callback.target<T>(); }),
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

// Explicit template instantiation
template void EventManager::AddListener<void>(EventType eventType, EventCallback callback);
template void EventManager::RemoveListener<void>(EventType eventType, EventCallback callback);

} // namespace RhyEngine