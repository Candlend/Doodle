#pragma once

#include "pch.h"

#include "UUID.h"

namespace Doodle
{

struct ProjectInfo
{
    std::string Name = "untitledProject";
    rfl::Rename<"StartScene", UUID> StartSceneUUID = UUID::Nil();
};

} // namespace Doodle