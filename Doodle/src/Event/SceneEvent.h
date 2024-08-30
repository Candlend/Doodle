#pragma once

#include "Event.h"

namespace Doodle
{

class Scene;
class DOO_API SceneActivateEvent : public Event
{
public:
    SceneActivateEvent(Scene &scene) : m_scene(scene)
    {
    }

    Scene &GetScene() const
    {
        return m_scene;
    }

    EVENT_CLASS_TYPE(SceneActivate)
    EVENT_CLASS_CATEGORY(EventCategoryScene)
private:
    Scene &m_scene;
};

class DOO_API SceneDeactivateEvent : public Event
{
public:
    SceneDeactivateEvent(Scene &scene) : m_scene(scene)
    {
    }

    Scene &GetScene() const
    {
        return m_scene;
    }

    EVENT_CLASS_TYPE(SceneDeactivate)
    EVENT_CLASS_CATEGORY(EventCategoryScene)
private:
    Scene &m_scene;
};

} // namespace Doodle