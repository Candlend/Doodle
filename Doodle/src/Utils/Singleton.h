#pragma once

#include "Log.h"
#include <mutex>
#include <typeinfo>

namespace Doodle
{

template <typename T> class Singleton
{
public:
    // 获取单例实例
    static std::shared_ptr<T> Get()
    {
        static std::shared_ptr<T> s_Instance = std::make_shared<T>();
        static std::once_flag s_Once;
        std::call_once(s_Once, []() { DOO_CORE_DEBUG("{0} Initialize", typeid(T).name()); });
        return s_Instance;
    }

    // 删除拷贝构造和赋值运算符
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;

protected:
    Singleton() = default;
    virtual ~Singleton()
    {
        m_destroyed = true;
    };

    bool m_destroyed = false;
};

} // namespace Doodle