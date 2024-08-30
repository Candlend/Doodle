#pragma once

#include "Entity.h"
#include <memory>

namespace Doodle
{

class DOO_API Scriptable
{
public:
    template <typename T> T &GetComponent()
    {
        return m_entity->GetComponent<T>();
    }

    Scriptable() : m_entity(nullptr)
    {
    }

protected:
    virtual ~Scriptable() = default;

    virtual void Initialize()
    {
    }

    virtual void Deinitialize()
    {
    }

    virtual void OnUpdate()
    {
    }

    virtual void OnLayout()
    {
    }

private:
    std::shared_ptr<Doodle::Entity> m_entity;
    friend class Scene;
};

} // namespace Doodle