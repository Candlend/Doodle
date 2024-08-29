#pragma once

#include "ScriptableEntity.h"

namespace Doodle
{

struct NativeScript
{
    ScriptableEntity *Instance = nullptr;

    std::function<void()> InstantiateFunction;
    std::function<void()> DestroyInstanceFunction; // TODO 没用到

    std::function<void(ScriptableEntity *)> InitializeFunction;
    std::function<void(ScriptableEntity *)> DeinitializeFunction; // TODO 没用到
    std::function<void(ScriptableEntity *)> OnUpdateFunction;

    template <typename T> void Bind()
    {
        InstantiateFunction = [&]() { Instance = new T(); };
        DestroyInstanceFunction = [&]() {
            delete static_cast<T *>(Instance);
            Instance = nullptr;
        };

        InitializeFunction = [](ScriptableEntity *instance) { static_cast<T *>(instance)->Initialize(); };
        DeinitializeFunction = [](ScriptableEntity *instance) { static_cast<T *>(instance)->Deinitialize(); };
        OnUpdateFunction = [](ScriptableEntity *instance) { static_cast<T *>(instance)->OnUpdate(); };
    }
};

} // namespace Doodle
