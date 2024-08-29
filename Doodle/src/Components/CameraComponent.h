#pragma once

#include "pch.h"

#include "SceneCamera.h"

namespace Doodle
{

struct CameraComponent
{
    std::shared_ptr<SceneCamera> Camera;
    bool Primary = true;

    explicit CameraComponent(ProjectionType type = ProjectionType::Perspective)
        : Camera(std::make_shared<SceneCamera>(type))
    {
    }

    glm::mat4 GetProjectionMatrix() const
    {
        return Camera->GetProjectionMatrix();
    }
};

} // namespace Doodle