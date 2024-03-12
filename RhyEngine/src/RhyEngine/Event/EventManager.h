#include "BaseEvent.h"
#include "EventType.h"
#include "RhyEngine/Core.h"
#include "RhyEngine/Common/Singleton.h"
#include "pch.h"


namespace RhyEngine
{

class RHY_API EventManager : public Singleton<EventManager>
{
public:
    using EventCallback = std::function<void(BaseEvent &)>;

    template <typename T> void AddListener(EventType eventType, EventCallback callback)
    {
        auto &listeners = m_eventListeners[eventType];
        listeners.push_back(callback);
    }

    template <typename T> void RemoveListener(EventType eventType, EventCallback callback)
    {
        auto &listeners = m_eventListeners[eventType];
        listeners.erase(
            std::remove_if(listeners.begin(), listeners.end(),
                           [&](const EventCallback &listener) { return listener.target<T>() == callback.target<T>(); }),
            listeners.end());
    }

    void Dispatch(BaseEvent &event)
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

private:
    std::unordered_map<EventType, std::vector<EventCallback>> m_eventListeners;
};

} // namespace RhyEngine
