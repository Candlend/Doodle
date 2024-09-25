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
    SceneActivateEvent(Scene *scene) : m_scene(scene)
    {
    }

    Scene *GetScene() const
    {
        return m_scene;
    }

    EVENT_CLASS_TYPE(SceneActivate)
private:
    Scene *m_scene;
};

class DOO_API SceneDeactivateEvent : public Event
{
public:
    SceneDeactivateEvent(Scene *scene) : m_scene(scene)
    {
    }

    Scene *GetScene() const
    {
        return m_scene;
    }

    EVENT_CLASS_TYPE(SceneDeactivate)
private:
    Scene *m_scene;
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
        ss << "SelectionChangedEvent: Context(" << static_cast<int32_t>(m_context) << "), Selection("
           << m_selectionId.ToString() << "), " << m_selected;
        return ss.str();
    }

    EVENT_CLASS_TYPE(SelectionChanged)
private:
    SelectionContext m_context;
    UUID m_selectionId;
    bool m_selected;
};

class DOO_API ViewportResizeEvent : public Event
{
public:
    ViewportResizeEvent(uint32_t width, uint32_t height) : m_width(width), m_height(height)
    {
    }

    uint32_t GetWidth() const
    {
        return m_width;
    }
    uint32_t GetHeight() const
    {
        return m_height;
    }

    EVENT_CLASS_TYPE(ViewportResize)
private:
    uint32_t m_width, m_height;
};

} // namespace Doodle