#pragma once

#include "Event.h"
#include "RhyEngine/Common/Singleton.h"
#include "RhyEngine/Core.h"

namespace RhyEngine
{

class RHY_API EventManager : public Singleton<EventManager>
{
public:
    using EventCallback = std::function<void(BaseEvent &)>;

    void AddListener(EventType eventType, EventCallback callback);
    void RemoveListener(EventType eventType, EventCallback callback);
    void Dispatch(BaseEvent &event);

private:
    std::unordered_map<EventType, std::vector<EventCallback>> m_eventListeners;
};

} // namespace RhyEngine