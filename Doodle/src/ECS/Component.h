#pragma once

#include "pch.h"
#include <glm/glm.hpp>

#include "VertexArray.h"

namespace Doodle
{

struct Transform
{
    glm::vec3 Position;
    glm::vec3 Rotation;
    glm::vec3 Scale;

    Transform() : Position(0.0f), Rotation(0.0f), Scale(1.0f)
    {
    }
};

struct VAOComponent
{
    std::shared_ptr<VertexArray> VAO;
};



} // namespace Doodle