#pragma once

#include <memory>
namespace Doodle
{
    
class Pimpl
{
protected:
    class Impl;
public:
    ~Pimpl() = default;

    template<typename fn_t>
    auto CallImpl(fn_t func) -> decltype(func())
    {
        if constexpr (std::is_same_v<void, decltype(func()) >)
        {
            func();
        }
        else
        {
            auto retval = func();
            return retval;
        }
    }

protected:
    std::unique_ptr<Impl> Impl;
};

} // namespace Doodle
