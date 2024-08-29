#pragma once

#include "Scriptable.h"

namespace Doodle
{

struct ScriptComponent
{
    Scriptable *Instance = nullptr;

    std::function<void()> InstantiateFunction;
    std::function<void()> DestroyInstanceFunction; // TODO 没用到

    std::function<void(Scriptable *)> InitializeFunction;
    std::function<void(Scriptable *)> DeinitializeFunction; // TODO 没用到
    std::function<void(Scriptable *)> OnUpdateFunction;
    std::function<void(Scriptable *)> OnLayoutFunction;

    template <typename T> void Bind()
    {
        InstantiateFunction = [&]() { Instance = new T(); };
        DestroyInstanceFunction = [&]() {
            delete static_cast<T *>(Instance);
            Instance = nullptr;
        };

        InitializeFunction = [](Scriptable *instance) { static_cast<T *>(instance)->Initialize(); };
        DeinitializeFunction = [](Scriptable *instance) { static_cast<T *>(instance)->Deinitialize(); };
        OnUpdateFunction = [](Scriptable *instance) { static_cast<T *>(instance)->OnUpdate(); };
        OnLayoutFunction = [](Scriptable *instance) { static_cast<T *>(instance)->OnLayout(); };
    }
};

} // namespace Doodle
