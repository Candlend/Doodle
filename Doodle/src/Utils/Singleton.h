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
    static T *Get()
    {
        static T s_Instance;
        static std::once_flag s_Once;
        std::call_once(s_Once, []() { DOO_CORE_DEBUG("{0} Initialize", typeid(T).name()); });
        return &s_Instance;
    }

    // 删除拷贝构造和赋值运算符
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;

protected:
    Singleton() = default;
    virtual ~Singleton() = default;
};

template <typename T> class LazySingleton : public std::enable_shared_from_this<T>
{
public:
    static std::shared_ptr<T> Get()
    {
        if (!s_Instance)
        {
            DOO_CORE_ERROR("{0} is not initialized", typeid(T).name());
        }
        return s_Instance;
    }

    // 删除拷贝构造和赋值运算符
    LazySingleton(const LazySingleton &) = delete;
    LazySingleton &operator=(const LazySingleton &) = delete;

protected:
    static void SetInstance(std::shared_ptr<T> instance)
    {
        s_Instance = instance;
    }
    static std::shared_ptr<T> s_Instance;
    LazySingleton() = default;
    virtual ~LazySingleton() = default;
};

template <typename T> std::shared_ptr<T> LazySingleton<T>::s_Instance = nullptr;

} // namespace Doodle