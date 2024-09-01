#pragma once

#include "Event.h"
#include "SelectionManager.h"
#include "UUID.h"

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

class SelectionChangedEvent : public Event
{
public:
    SelectionChangedEvent(SelectionContext contextID, UUID selectionID, bool selected)
        : m_context(contextID), m_selectionId(selectionID), m_selected(selected)
    {
    }

    SelectionContext GetContextID() const
    {
        return m_context;
    }
    UUID GetSelectionID() const
    {
        return m_selectionId;
    }
    bool IsSelected() const
    {
        return m_selected;
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "SelectionChangedEvent: Context(" << static_cast<int32_t>(m_context) << "), Selection(" << m_selectionId
           << "), " << m_selected;
        return ss.str();
    }

    EVENT_CLASS_CATEGORY(EventCategoryScene)
    EVENT_CLASS_TYPE(SelectionChanged)
private:
    SelectionContext m_context;
    UUID m_selectionId;
    bool m_selected;
};

} // namespace Doodle