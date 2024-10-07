#pragma once

#include "pch.h"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Doodle
{

struct Transform
{
    glm::vec3 Position;
    glm::vec3 Rotation; // Euler angles in degrees
    glm::vec3 Scale;

    Transform() : Position(0.0f), Rotation(0.0f), Scale(1.0f)
    {
    }

    Transform(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale)
        : Position(position), Rotation(rotation), Scale(scale)
    {
    }

    glm::quat GetQuaternion() const;

    glm::mat4 GetTransformMatrix() const;

    glm::vec3 GetFront() const;

    glm::vec3 GetRight() const;

    glm::vec3 GetUp() const;

    void LookAt(const glm::vec3 &target);

    void Rotate(const glm::vec3 &eulerAngles);

    void Translate(const glm::vec3 &translation)
    {
        Position += translation;
    }

    void ScaleBy(const glm::vec3 &scale)
    {
        Scale *= scale;
    }

    void ScaleBy(float scale)
    {
        Scale *= scale;
    }

    void Reset();
};

} // namespace Doodle