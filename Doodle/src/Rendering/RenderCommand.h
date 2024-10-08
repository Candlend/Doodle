#pragma once

#include "pch.h"
#include <functional>

namespace Doodle
{

template <typename... Args> class RenderCommand
{
public:
    using ExecuteFn = std::function<void(Args...)>;

    RenderCommand(ExecuteFn func, Args... args) : m_func(func), m_args(std::make_tuple(std::forward<Args>(args)...))
    {
    }

    void Execute()
    {
        std::apply(m_func, m_args);
    }

private:
    ExecuteFn m_func;
    std::tuple<typename std::decay<Args>::type...> m_args;
};

} // namespace Doodle
