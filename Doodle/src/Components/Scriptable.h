#pragma once

#include "Entity.h"
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

    virtual ~Scriptable();

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
