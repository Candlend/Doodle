#pragma once

namespace RhyEngine
{

template <typename T>
class Singleton
{
public:
    // 获取单例实例
    static T &Get(){
        static T s_Instance;
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