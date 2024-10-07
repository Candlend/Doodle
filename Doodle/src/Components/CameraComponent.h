#pragma once

#include "pch.h"

#include "BaseComponent.h"
#include "SceneCamera.h"
#include "rfl/enums.hpp"

namespace Doodle
{

struct CameraComponent : public BaseComponent
{
    COMPONENT_CLASS_TYPE(Camera)

    std::shared_ptr<SceneCamera> Camera;
    bool Primary = true;

    CameraComponent(ProjectionType type = ProjectionType::Perspective) : Camera(std::make_shared<SceneCamera>(type))
    {
    }
    glm::mat4 GetProjectionMatrix() const
    {
        return Camera->GetProjectionMatrix();
    }

    operator std::shared_ptr<SceneCamera> &()
    {
        return Camera;
    }

    operator const std::shared_ptr<SceneCamera> &() const
    {
        return Camera;
    }

    void OnInspectorLayout() override;

    rfl::Generic::Object SerializeToObject() const override;

    void DeserializeFromObject(const rfl::Generic::Object &object) override;
};

} // namespace Doodle