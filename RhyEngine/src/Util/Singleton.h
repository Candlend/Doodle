#pragma once

#include "Log.h"
#include <mutex>
#include <typeinfo>

namespace RhyEngine
{

template <typename T>
class Singleton
{
public:
    // 获取单例实例
    static T &Get(){
        static T s_Instance;
        static std::once_flag s_Once;
        std::call_once(s_Once, []() {
            RHY_CORE_INFO("{0} Init", typeid(T).name());
        });
        return s_Instance;
    }

    // 删除拷贝构造和赋值运算符
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;

protected:
    Singleton() = default;
    virtual ~Singleton() = default;
};

} // namespace RhyEngine