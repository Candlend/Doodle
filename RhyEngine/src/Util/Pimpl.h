#pragma once

#include <memory>
namespace RhyEngine
{
    
class Pimpl
{
protected:
    class Impl;
public:
    ~Pimpl() = default;

    template<typename fn_t>
    auto CallImpl(fn_t fn) -> decltype(fn())
    {
        if constexpr (std::is_same_v<void, decltype(fn()) >)
        {
            fn();
        }
        else
        {
            auto retval = fn();
            return retval;
        }
    }

protected:
    std::unique_ptr<Impl> m_pImpl;
};

} // namespace RhyEngine
