#pragma once

#include "pch.h"

#include "Core.h"
#include "EventManager.h"

namespace RhyEngine
{

class RHY_API BaseLayer
{
public:
    explicit BaseLayer(const std::string &name = "Layer") : m_debugName(name) {};
    virtual ~BaseLayer() = default;

    virtual void OnAttach() = 0;
    virtual void OnDetach() = 0;
    virtual void OnUpdate() = 0;
    void OnEvent(BaseEvent &event)
    {
        m_eventManager.Dispatch(event);
    }

    inline const std::string &GetName() const
    {
        return m_debugName;
    }

protected:
    std::string m_debugName;
    EventManager m_eventManager;
};

} // namespace RhyEngine