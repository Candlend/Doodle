#pragma once

#include "pch.h"

#include "Core.h"
#include "Event.h"
#include "Singleton.h"
#include <limits>

namespace Doodle
{

template <typename T> using EventCallbackFn = std::function<bool(T &)>;

struct EventCallback
{
    void *Obj;
    void *Func;
    EventCallbackFn<Event> Callback;
    int ExecutionOrder; // 新增优先级成员
};
// NOLINTBEGIN
template <typename T, typename R, typename E> void *void_cast(R (T::*f)(E))
{
    union {
        R (T::*pf)(E);
        void *p;
    };
    pf = f;
    return p;
}
// NOLINTEND

// NOLINTBEGIN
template <typename T> void *void_cast(void (T::*f)())
{
    union {
        void (T::*pf)();
        void *p;
    };
    pf = f;
    return p;
}
// NOLINTEND
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

class DOO_API EventManager : public Singleton<EventManager>
{
public:
    ~EventManager()
    {
        m_eventListeners.clear();
    }
    template <typename T> void AddListener(EventCallbackFn<T> &callback, int executionOrder = 0)
    {
        EventType eventType = T::GetStaticType();
        auto &listeners = m_eventListeners[eventType];
        DOO_DEBUG("AddListener {0}", typeid(callback).name());
        EventCallback eventCallback{nullptr, reinterpret_cast<void *>(&callback),
                                    [callback](Event &event) -> bool { return callback(static_cast<T &>(event)); },
                                    executionOrder}; // 设置优先级

        listeners.push_back(eventCallback);
    }

    template <typename T> void AddListener(std::function<void()> &callback, int executionOrder = 0)
    {
        EventType eventType = T::GetStaticType();
        auto &listeners = m_eventListeners[eventType];
        DOO_DEBUG("AddListener {0}", typeid(callback).name());
        EventCallback eventCallback{nullptr, reinterpret_cast<void *>(&callback),
                                    [callback](Event & /*event*/) {
                                        callback();
                                        return false;
                                    },
                                    executionOrder = 0}; // 设置优先级

        listeners.push_back(eventCallback);
    }

    template <typename T, typename C> void AddListener(C *obj, bool (C::*func)(T &), int executionOrder = 0)
    {
        EventType eventType = T::GetStaticType();
        auto callback = [obj, func](Event &event) -> bool { return (obj->*func)(static_cast<T &>(event)); };
        auto &listeners = m_eventListeners[eventType];
        DOO_DEBUG("AddListener {0}", typeid(callback).name());
        EventCallback eventCallback{reinterpret_cast<void *>(obj), void_cast(func), callback,
                                    executionOrder}; // 设置优先级
        listeners.emplace_back(eventCallback);
    }

    template <typename T, typename C> void AddListener(C *obj, void (C::*func)(), int executionOrder = 0)
    {
        EventType eventType = T::GetStaticType();
        auto callback = [obj, func](Event & /*event*/) {
            (obj->*func)();
            return false;
        };
        auto &listeners = m_eventListeners[eventType];
        DOO_DEBUG("AddListener {0}", typeid(callback).name());
        EventCallback eventCallback{reinterpret_cast<void *>(obj), void_cast(func), callback,
                                    executionOrder}; // 设置优先级
        listeners.emplace_back(eventCallback);
    }

    template <typename T> void RemoveListener(EventCallbackFn<T> &callback)
    {
        if (m_destroyed) return;
        EventType eventType = T::GetStaticType();
        auto &listeners = m_eventListeners[eventType];

        DOO_DEBUG("RemoveListener {0}", typeid(callback).name());

        // 使用 std::remove_if 和比较函数进行移除
        auto it = std::remove_if(listeners.begin(), listeners.end(), [&](const EventCallback &listener) {
            return listener.Obj == nullptr && listener.Func == reinterpret_cast<void *>(&callback);
        });

        listeners.erase(it, listeners.end());
    }

    template <typename T> void RemoveListener(std::function<void()> &callback)
    {
        if (m_destroyed) return;
        EventType eventType = T::GetStaticType();
        auto &listeners = m_eventListeners[eventType];

        DOO_DEBUG("RemoveListener {0}", typeid(callback).name());

        // 使用 std::remove_if 和比较函数进行移除
        auto it = std::remove_if(listeners.begin(), listeners.end(), [&](const EventCallback &listener) {
            return listener.Obj == nullptr && listener.Func == reinterpret_cast<void *>(&callback);
        });

        listeners.erase(it, listeners.end());
    }

    template <typename T, typename C> void RemoveListener(C *obj, bool (C::*func)(T &))
    {
        if (m_destroyed) return;
        EventType eventType = T::GetStaticType();
        auto &listeners = m_eventListeners[eventType];
        DOO_DEBUG("RemoveListener {0}", typeid(func).name());
        void *a = void_cast(func);
        auto it = std::remove_if(listeners.begin(), listeners.end(), [&](const EventCallback &listener) {
            return listener.Obj == static_cast<void *>(obj) && listener.Func == void_cast(func);
        });

        listeners.erase(it, listeners.end());
    }

    template <typename T, typename C> void RemoveListener(C *obj, void (C::*func)())
    {
        if (m_destroyed) return;
        EventType eventType = T::GetStaticType();
        auto &listeners = m_eventListeners[eventType];
        DOO_DEBUG("RemoveListener {0}", typeid(func).name());
        void *a = void_cast(func);
        auto it = std::remove_if(listeners.begin(), listeners.end(), [&](const EventCallback &listener) {
            return listener.Obj == static_cast<void *>(obj) && listener.Func == void_cast(func);
        });

        listeners.erase(it, listeners.end());
    }

    void Dispatch(Event &event);

    template <typename TEvent, typename... TEventArgs> void Dispatch(TEventArgs &&...args)
    {
        TEvent event(std::forward<TEventArgs>(args)...);
        Dispatch(event);
    }

private:
    std::unordered_map<EventType, std::vector<EventCallback>> m_eventListeners;
};

enum ExecutionOrder
{
    First = std::numeric_limits<int>::min(),
    Early = -1000,
    Default = 0,
    Late = 1000,
    Last = std::numeric_limits<int>::max()
};
} // namespace Doodle
