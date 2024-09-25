#pragma once

#include "pch.h"

#include "BaseComponent.h"
#include "SceneCamera.h"

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
};

} // namespace Doodle

using namespace Doodle;

namespace rfl
{

template <> struct Reflector<CameraComponent>
{
    struct ReflType
    {
        bool Primary;
        ProjectionType Projection;
        float Fov;
        float NearClip;
        float FarClip;
        float Size;
        float OrthoNearClip;
        float OrthoFarClip;
    };

    static CameraComponent to(const ReflType &v) noexcept // NOLINT
    {
        CameraComponent component(v.Projection);
        component.Primary = v.Primary;
        if (v.Projection == ProjectionType::Perspective)
        {
            component.Camera->SetDegPerspectiveVerticalFov(v.Fov);
            component.Camera->SetPerspectiveNearClip(v.NearClip);
            component.Camera->SetPerspectiveFarClip(v.FarClip);
        }
        else
        {
            component.Camera->SetOrthographicSize(v.Size);
            component.Camera->SetOrthographicNearClip(v.OrthoNearClip);
            component.Camera->SetOrthographicFarClip(v.OrthoFarClip);
        }
        return component;
    }

    static ReflType from(const CameraComponent &v) noexcept // NOLINT
    {
        ReflType component;
        component.Primary = v.Primary;
        component.Projection = v.Camera->GetProjectionType();
        if (component.Projection == ProjectionType::Perspective)
        {
            component.Fov = v.Camera->GetDegPerspectiveVerticalFov();
            component.NearClip = v.Camera->GetPerspectiveNearClip();
            component.FarClip = v.Camera->GetPerspectiveFarClip();
        }
        else
        {
            component.Size = v.Camera->GetOrthographicSize();
            component.OrthoNearClip = v.Camera->GetOrthographicNearClip();
            component.OrthoFarClip = v.Camera->GetOrthographicFarClip();
        }
        return component;
    }
};

} // namespace rfl