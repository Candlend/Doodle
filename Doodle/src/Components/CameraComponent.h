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

    void OnInspectorLayout() override
    {
        ImGui::Checkbox("Primary", &Primary);
        ImGui::Text("Projection");
        ImGui::SameLine();
        if (ImGui::RadioButton("Perspective", Camera->GetProjectionType() == ProjectionType::Perspective))
        {
            Camera->SetProjectionType(ProjectionType::Perspective);
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Orthographic", Camera->GetProjectionType() == ProjectionType::Orthographic))
        {
            Camera->SetProjectionType(ProjectionType::Orthographic);
        }
        if (Camera->GetProjectionType() == ProjectionType::Perspective)
        {
            float fov = Camera->GetDegPerspectiveVerticalFov();
            if (ImGui::DragFloat("FOV", &fov, 1.0f, 1.0f, 179.0f))
            {
                Camera->SetDegPerspectiveVerticalFov(fov);
            }
            float nearClip = Camera->GetPerspectiveNearClip();
            if (ImGui::DragFloat("Near Clip", &nearClip, 0.1f, 0.0f, Camera->GetPerspectiveFarClip()))
            {
                Camera->SetPerspectiveNearClip(nearClip);
            }
            float farClip = Camera->GetPerspectiveFarClip();
            if (ImGui::DragFloat("Far Clip", &farClip, 0.1f, Camera->GetPerspectiveNearClip(), 1000.0f))
            {
                Camera->SetPerspectiveFarClip(farClip);
            }
        }
        else
        {
            float size = Camera->GetOrthographicSize();
            if (ImGui::DragFloat("Size", &size, 0.1f, 0.1f, 100.0f))
            {
                Camera->SetOrthographicSize(size);
            }
            float nearClip = Camera->GetOrthographicNearClip();
            if (ImGui::DragFloat("Near Clip", &nearClip, 0.1f, 0.0f, Camera->GetOrthographicFarClip()))
            {
                Camera->SetOrthographicNearClip(nearClip);
            }
            float farClip = Camera->GetOrthographicFarClip();
            if (ImGui::DragFloat("Far Clip", &farClip, 0.1f, Camera->GetOrthographicNearClip(), 1000.0f))
            {
            }
        };
    }

    rfl::Generic::Object SerializeToObject() const override
    {
        rfl::Generic::Object object;
        object["Primary"] = Primary;
        object["Projection"] = rfl::enum_to_string(Camera->GetProjectionType());
        if (Camera->GetProjectionType() == ProjectionType::Perspective)
        {
            object["Fov"] = Camera->GetDegPerspectiveVerticalFov();
            object["NearClip"] = Camera->GetPerspectiveNearClip();
            object["FarClip"] = Camera->GetPerspectiveFarClip();
        }
        else
        {
            object["Size"] = Camera->GetOrthographicSize();
            object["OrthoNearClip"] = Camera->GetOrthographicNearClip();
            object["OrthoFarClip"] = Camera->GetOrthographicFarClip();
        }
        return object;
    }

    void DeserializeFromObject(const rfl::Generic::Object &object) override
    {
        Primary = object.get("Primary").and_then(rfl::to_bool).value();
        Camera->SetProjectionType(
            rfl::string_to_enum<ProjectionType>(object.get("Projection").and_then(rfl::to_string).value()).value());
        if (Camera->GetProjectionType() == ProjectionType::Perspective)
        {
            Camera->SetDegPerspectiveVerticalFov(object.get("Fov").and_then(rfl::to_double).value());
            Camera->SetPerspectiveNearClip(object.get("NearClip").and_then(rfl::to_double).value());
            Camera->SetPerspectiveFarClip(object.get("FarClip").and_then(rfl::to_double).value());
        }
        else
        {
            Camera->SetOrthographicSize(object.get("Size").and_then(rfl::to_double).value());
            Camera->SetOrthographicNearClip(object.get("OrthoNearClip").and_then(rfl::to_double).value());
            Camera->SetOrthographicFarClip(object.get("OrthoFarClip").and_then(rfl::to_double).value());
        }
    }
};

} // namespace Doodle