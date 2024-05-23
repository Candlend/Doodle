#pragma once

#include "pch.h"

#include "Singleton.h"

namespace Doodle
{

class DOO_API Module
{
public:
    virtual ~Module() = default;

    virtual void Initialize() = 0;
    virtual void OnUpdate() = 0;
    virtual void OnLayout() = 0;
    virtual void Deinitialize() = 0;

    int GetExecutionOrder() const
    {
        return m_executionOrder;
    }

    static std::vector<Module *> &GetModules()
    {
        static std::vector<Module *> s_Modules;
        return s_Modules;
    }

protected:
    explicit Module(int executionOrder) : m_executionOrder(executionOrder)
    {
    }

private:
    int m_executionOrder;
};

template <typename T, int N = 0> class RegisterModule : public Module, public Singleton<T>
{
public:
    static void Register()
    {
        GetModules().push_back(&T::Get());
        std::sort(GetModules().begin(), GetModules().end(),
                  [](Module *a, Module *b) { return a->GetExecutionOrder() < b->GetExecutionOrder(); });
    } 

    RegisterModule() : Module(N)
    {
    }

};

} // namespace Doodle