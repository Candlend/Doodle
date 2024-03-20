#pragma once

#include "RhyEngine/Core.h"
#include "pch.h"

namespace RhyEngine
{

template <typename T> class Singleton
{
public:
    static T *s_Instance;

    // 获取单例实例
    static T &GetInstance()
    {
        if (s_Instance == nullptr)
        {
            s_Instance = new T();
        }
        return *static_cast<T*>(s_Instance);
    }

    // 删除拷贝构造和赋值运算符
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;

protected:
    Singleton() = default;
    virtual ~Singleton()
    {
        if (s_Instance != nullptr) delete s_Instance;
    }
};

// 初始化静态成员变量
template <typename T> T *Singleton<T>::s_Instance = nullptr;

} // namespace RhyEngine