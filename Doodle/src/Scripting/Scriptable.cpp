#include "Scriptable.h"
#include "Log.h"
#include "Scene.h"
#include "SceneEvent.h"
#include "SceneManager.h"

namespace Doodle
{

Scriptable::Scriptable()
{
}
Scriptable::~Scriptable()
{
    DOO_CORE_DEBUG("Scriptable destroyed");
    EventManager::Get()->RemoveListener(this, &Scriptable::OnSceneDeactivate);
    EventManager::Get()->RemoveListener<AppUpdateEvent>(this, &Scriptable::OnUpdate);
    EventManager::Get()->RemoveListener<AppLayoutEvent>(this, &Scriptable::OnLayout);
    EventManager::Get()->RemoveListener<AppRenderEvent>(this, &Scriptable::OnRender);
}

void Scriptable::OnAdded()
{
    EventManager::Get()->AddListener(this, &Scriptable::OnSceneActivate);
    if (IsValid() && GetScene()->IsActive() && SceneManager::Get()->GetState() == SceneState::Runtime)
    {
        EventManager::Get()->AddListener<AppUpdateEvent>(this, &Scriptable::OnUpdate);
        EventManager::Get()->AddListener<AppLayoutEvent>(this, &Scriptable::OnLayout);
        EventManager::Get()->AddListener<AppRenderEvent>(this, &Scriptable::OnRender);
        Initialize();
    }
}

bool Scriptable::OnSceneActivate(const SceneActivateEvent &e)
{
    if (e.GetScene() == *GetScene() && IsValid())
    {
        EventManager::Get()->AddListener<AppUpdateEvent>(this, &Scriptable::OnUpdate);
        EventManager::Get()->AddListener<AppLayoutEvent>(this, &Scriptable::OnLayout);
        EventManager::Get()->AddListener<AppRenderEvent>(this, &Scriptable::OnRender);
        Initialize();
    }
    return false;
}

bool Scriptable::OnSceneDeactivate(const SceneDeactivateEvent &e)
{
    if (e.GetScene() == *GetScene() && IsValid())
    {
        Deinitialize();
        EventManager::Get()->RemoveListener<AppUpdateEvent>(this, &Scriptable::OnUpdate);
        EventManager::Get()->RemoveListener<AppLayoutEvent>(this, &Scriptable::OnLayout);
        EventManager::Get()->RemoveListener<AppRenderEvent>(this, &Scriptable::OnRender);
    }
    return false;
}
} // namespace Doodle
