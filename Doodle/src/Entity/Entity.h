#pragma once

#include "pch.h"
#include <entt/entt.hpp>

namespace Doodle
{

class DOO_API Entity
{
public:
    explicit Entity(entt::registry &registry);
    template <typename T, typename... Args> T &AddComponent(Args &&...args)
    {
        return m_registry.emplace<T>(m_id, std::forward<Args>(args)...);
    }

    template <typename T> T &GetComponent()
    {
        return m_registry.get<T>(m_id);
    }

    template <typename T> bool HasComponent()
    {
        return m_registry.all_of<T>(m_id);
    }
    entt::entity GetId() const;

private:
    entt::registry &m_registry;
    entt::entity m_id;
};

} // namespace Doodle