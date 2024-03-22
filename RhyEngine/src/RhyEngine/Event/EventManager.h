#pragma once

#include "Event.h"
#include "RhyEngine/Core.h"
#include <algorithm>
#include <unordered_map>
#include <vector>

namespace RhyEngine
{

template <typename T>
using EventCallbackFn = std::function<bool(T &)>;

struct EventCallback
{
    void *Obj;
    void *Func;
    EventCallbackFn<BaseEvent> Callback;
};

template <typename T, typename R, typename E> void *void_cast(R (T::*f)(E))
{
    union {
        R (T::*pf)(E);
        void *p;
    };
    pf = f;
    return p;
}

struct PairHash
{
    template <class T1, class T2> std::size_t operator()(const std::pair<T1, T2> &pair) const
    {
        auto hash1 = std::hash<T1>{}(pair.first);
        auto hash2 = std::hash<T2>{}(pair.second);
        return hash1 ^ (hash2 << 1); // Combine the two hash values
    }
};

struct PairEqual
{
    template <class T1, class T2> bool operator()(const std::pair<T1, T2> &lhs, const std::pair<T1, T2> &rhs) const
    {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }
};

class RHY_API EventManager
{
public:
    template <typename T> void AddListener(EventCallbackFn<T> &callback);

    template <typename T, typename C> void AddListener(C *obj, bool (C::*func)(T &));

    template <typename T> void RemoveListener(EventCallbackFn<T> &callback);

    template <typename T, typename C> void RemoveListener(C *obj, bool (C::*func)(T &));

    void Dispatch(BaseEvent &event);

private:
    std::unordered_map<EventType, std::vector<EventCallback>> m_eventListeners;
};

template <typename T> void EventManager::AddListener(EventCallbackFn<T> &callback)
{
    EventType eventType = T::GetStaticType();
    auto &listeners = m_eventListeners[eventType];
    std::ostringstream oss;
    oss << &callback;
    std::string ptrStr = oss.str();
    RHY_DEBUG("AddListener {0} {1}", typeid(callback).name(), ptrStr);
    EventCallback eventCallback{nullptr, reinterpret_cast<void *>(&callback), [callback](BaseEvent &event) -> bool {
        return callback(static_cast<T &>(event));
    }};
    listeners.push_back(eventCallback);
}

template <typename T, typename C> void EventManager::AddListener(C *obj, bool (C::*func)(T &))
{
    EventType eventType = T::GetStaticType();
    auto callback = [obj, func](BaseEvent &event) -> bool { return (obj->*func)(static_cast<T &>(event)); };
    auto &listeners = m_eventListeners[eventType];
    RHY_DEBUG("AddListener {0}", typeid(callback).name());
    auto a = void_cast(func);
    EventCallback eventCallback{reinterpret_cast<void *>(obj), void_cast(func), callback};
    listeners.emplace_back(eventCallback);
}

template <typename T> void EventManager::RemoveListener(EventCallbackFn<T> &callback)
{
    EventType eventType = T::GetStaticType();
    auto &listeners = m_eventListeners[eventType];

    RHY_DEBUG("RemoveListener {0}", typeid(callback).name());

    // 使用 std::remove_if 和比较函数进行移除
    auto it = std::remove_if(listeners.begin(), listeners.end(), [&](const EventCallback &listener) {
        return listener.Obj == nullptr && listener.Func == reinterpret_cast<void *>(&callback);
    });

    listeners.erase(it, listeners.end());
}

template <typename T, typename C> void EventManager::RemoveListener(C *obj, bool (C::*func)(T &))
{
    EventType eventType = T::GetStaticType();
    auto &listeners = m_eventListeners[eventType];
    void *a = void_cast(func);
    auto it = std::remove_if(listeners.begin(), listeners.end(), [&](const EventCallback &listener) {
        return listener.Obj == static_cast<void *>(obj) && listener.Func == void_cast(func);
    });

    listeners.erase(it, listeners.end());
}

} // namespace RhyEngine
