#pragma once

#include "SceneEvent.h"
#include "pch.h"

#include "ApplicationEvent.h"
#include "BaseComponent.h"
#include "EventManager.h"

namespace Doodle
{

class SceneActivateEvent;
class SceneDeactivateEvent;

class DOO_API Scriptable : public BaseComponent
{
public:
    Scriptable();

    virtual ~Scriptable() override;

    // 不允许拷贝构造和赋值
    Scriptable(const Scriptable &) = delete;
    Scriptable &operator=(const Scriptable &) = delete;

    void OnAdded();
    virtual void Initialize() {};
    virtual void OnUpdate() {};
    virtual void OnLayout() {};
    virtual void OnRender() {};
    virtual void Deinitialize() {};

private:
    bool OnSceneActivate(const SceneActivateEvent &e);
    bool OnSceneDeactivate(const SceneDeactivateEvent &e);
};

} // namespace Doodle
