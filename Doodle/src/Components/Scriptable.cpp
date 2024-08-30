#include "Scriptable.h"
#include "Scene.h"
#include "SceneEvent.h"

namespace Doodle
{

Scriptable::Scriptable()
{
    EventManager::Get()->AddListener(this, &Scriptable::OnSceneActivate);
    EventManager::Get()->AddListener<AppUpdateEvent>(this, &Scriptable::OnUpdate);
    EventManager::Get()->AddListener<AppLayoutEvent>(this, &Scriptable::OnLayout);
    EventManager::Get()->AddListener<AppRenderEvent>(this, &Scriptable::OnRender);
    DOO_CORE_TRACE("Scriptable created");
}
Scriptable::~Scriptable()
{
    EventManager::Get()->RemoveListener(this, &Scriptable::OnSceneDeactivate);
    EventManager::Get()->RemoveListener<AppUpdateEvent>(this, &Scriptable::OnUpdate);
    EventManager::Get()->RemoveListener<AppLayoutEvent>(this, &Scriptable::OnLayout);
    EventManager::Get()->RemoveListener<AppRenderEvent>(this, &Scriptable::OnRender);
    DOO_CORE_TRACE("Scriptable destroyed");
}

bool Scriptable::OnSceneActivate(const SceneActivateEvent &e)
{
    if (e.GetScene() == *GetScene() && IsValid())
    {
        Initialize();
    }
    return false;
}

bool Scriptable::OnSceneDeactivate(const SceneDeactivateEvent &e)
{
    if (e.GetScene() == *GetScene() && IsValid())
    {
        Deinitialize();
    }
    return false;
}
} // namespace Doodle
