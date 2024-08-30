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
        // 按优先级排序
        std::sort(it->second.begin(), it->second.end(),
                  [](const EventCallback &a, const EventCallback &b) { return a.ExecutionOrder < b.ExecutionOrder; });

        for (auto &listener : it->second)
        {
            // 调用回调并将返回值赋值给 Handled
            if (listener.Callback(event))
            {
                event.Handled = true; // 如果回调返回 true，设置 Handled 为 true
                break; // 停止传递给下一个回调
            }
        }
    }
}


} // namespace Doodle