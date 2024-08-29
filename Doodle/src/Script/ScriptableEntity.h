#pragma once

#include "Entity.h"
#include <memory>

namespace Doodle
{

class DOO_API ScriptableEntity
{
public:
    template <typename T> T &GetComponent()
    {
        return m_entity->GetComponent<T>();
    }

    explicit ScriptableEntity() : m_entity(nullptr)
    {
    }

protected:
    virtual ~ScriptableEntity() = default;

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