#pragma once

#include "pch.h"

#include "UUID.h"

namespace Doodle
{

struct EntityInfo
{
    UUID UUID;
    std::string Name;
    rfl::Generic::Object Components;
    std::vector<EntityInfo> Children;
};

struct SceneInfo
{
    UUID UUID;
    std::vector<EntityInfo> Entities;
};

} // namespace Doodle