#pragma once

#include "pch.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include <imGuizmo.h>

#include "BaseComponent.h"
#include "ImGuiUtils.h"
#include "MathUtils.h"

namespace Doodle
{

struct DirectionalLightComponent : public BaseComponent
{
    COMPONENT_CLASS_TYPE(DirectionalLight)

    glm::vec3 Radiance{1.0f};
    float Intensity = 0.0f;

    DirectionalLightComponent() = default;
    DirectionalLightComponent(const glm::vec3 &radiance, float intensity) : Radiance(radiance), Intensity(intensity)
    {
    }

    void OnInspectorLayout() override
    {
        ImGui::ColorEdit3("Radiance", glm::value_ptr(Radiance));
        ImGui::DragFloat("Intensity", &Intensity, 0.1f, 0.0f, 100.0f);
    }

    void OnDrawGizmos(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) override
    {
        ImU32 color = ImGuiUtils::GetHexColor(ImColor(Radiance.x, Radiance.y, Radiance.z));
        glm::mat4 modelNoScale = RemoveScaling(model);
        ImGuizmo::DrawDirectionalLightGizmos(glm::value_ptr(view), glm::value_ptr(projection),
                                             glm::value_ptr(modelNoScale), 1, color, 0.3f, 1.0f);
    }
};

struct PointLightComponent : public BaseComponent
{
    COMPONENT_CLASS_TYPE(PointLight)

    glm::vec3 Radiance{1.0f};
    float Intensity = 1.0f;
    float MinRange = 0.001f;
    float Range = 10.f;

    PointLightComponent() = default;
    PointLightComponent(const glm::vec3 &radiance, float intensity, float minRange, float range)
        : Radiance(radiance), Intensity(intensity), MinRange(minRange), Range(range)
    {
    }

    void OnInspectorLayout() override
    {
        ImGui::ColorEdit3("Radiance", glm::value_ptr(Radiance));
        ImGui::DragFloat("Intensity", &Intensity, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloatRange2("Range", &MinRange, &Range, 0.1f, 0.0f, 100.0f);
    }

    void OnDrawGizmos(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) override
    {
        ImU32 color = ImGuiUtils::GetHexColor(ImColor(Radiance.x, Radiance.y, Radiance.z));
        glm::mat4 modelNoScale = RemoveScaling(model);
        ImGuizmo::DrawPointLightGizmos(glm::value_ptr(view), glm::value_ptr(projection), glm::value_ptr(modelNoScale),
                                       1, color, Range);
    }
};

struct SpotLightComponent : public BaseComponent
{
    COMPONENT_CLASS_TYPE(SpotLight)

    glm::vec3 Radiance{1.0f};
    float Intensity = 1.0f;
    float MinRange = 0.001f;
    float Range = 10.f;
    float MinAngle = 0.01f;
    float Angle = 30.f;

    SpotLightComponent() = default;
    SpotLightComponent(const glm::vec3 &radiance, float intensity, float minRange, float range, float minAngle,
                       float angle)
        : Radiance(radiance), Intensity(intensity), MinRange(minRange), Range(range), MinAngle(minAngle), Angle(angle)
    {
    }

    void OnInspectorLayout() override
    {
        ImGui::ColorEdit3("Radiance", glm::value_ptr(Radiance));
        ImGui::DragFloat("Intensity", &Intensity, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloatRange2("Range", &MinRange, &Range, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloatRange2("Angle", &MinAngle, &Angle, 0.1f, 0.0f, 180.0f);
    }

    void OnDrawGizmos(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) override
    {
        ImU32 color = ImGuiUtils::GetHexColor(ImColor(Radiance.x, Radiance.y, Radiance.z));
        glm::mat4 modelNoScale = RemoveScaling(model);
        ImGuizmo::DrawSpotLightGizmos(glm::value_ptr(view), glm::value_ptr(projection), glm::value_ptr(model), 1, color,
                                      Range, Angle);
    }
};

struct AreaLightComponent : public BaseComponent
{
    COMPONENT_CLASS_TYPE(AreaLight)

    glm::vec3 Radiance{1.0f};
    float Intensity = 1.0f;
    glm::vec2 Size{1.0f};
    bool TwoSided = false;

    AreaLightComponent() = default;
    AreaLightComponent(const glm::vec3 &radiance, float intensity, const glm::vec2 &size, bool twoSided)
        : Radiance(radiance), Intensity(intensity), Size(size), TwoSided(twoSided)
    {
    }

    void OnInspectorLayout() override
    {
        ImGui::ColorEdit3("Radiance", glm::value_ptr(Radiance));
        ImGui::DragFloat("Intensity", &Intensity, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat2("Size", glm::value_ptr(Size), 0.1f, 0.0f, 100.0f);
        ImGui::Checkbox("Two Sided", &TwoSided);
    }

    void OnDrawGizmos(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) override
    {
        ImU32 color = ImGuiUtils::GetHexColor(ImColor(Radiance.x, Radiance.y, Radiance.z));
        ImGuizmo::DrawAreaLightGizmos(glm::value_ptr(view), glm::value_ptr(projection), glm::value_ptr(model), 1, color,
                                      Size.x, Size.y);
    }
};

} // namespace Doodle

using namespace Doodle;
namespace rfl
{

template <> struct Reflector<DirectionalLightComponent>
{
    struct ReflType
    {
        glm::vec3 Radiance;
        float Intensity;
    };

    static DirectionalLightComponent to(const ReflType &v) noexcept // NOLINT
    {
        return {v.Radiance, v.Intensity};
    }

    static ReflType from(const DirectionalLightComponent &v) noexcept // NOLINT
    {
        return {v.Radiance, v.Intensity};
    }
};

template <> struct Reflector<PointLightComponent>
{
    struct ReflType
    {
        glm::vec3 Radiance;
        float Intensity;
        float MinRange;
        float Range;
    };

    static PointLightComponent to(const ReflType &v) noexcept // NOLINT
    {
        return {v.Radiance, v.Intensity, v.MinRange, v.Range};
    }

    static ReflType from(const PointLightComponent &v) noexcept // NOLINT
    {
        return {v.Radiance, v.Intensity, v.MinRange, v.Range};
    }
};

template <> struct Reflector<SpotLightComponent>
{
    struct ReflType
    {
        glm::vec3 Radiance;
        float Intensity;
        float MinRange;
        float Range;
        float MinAngle;
        float Angle;
    };

    static SpotLightComponent to(const ReflType &v) noexcept // NOLINT
    {
        return {v.Radiance, v.Intensity, v.MinRange, v.Range, v.MinAngle, v.Angle};
    }

    static ReflType from(const SpotLightComponent &v) noexcept // NOLINT
    {
        return {v.Radiance, v.Intensity, v.MinRange, v.Range, v.MinAngle, v.Angle};
    }
};

template <> struct Reflector<AreaLightComponent>
{
    struct ReflType
    {
        glm::vec3 Radiance;
        float Intensity;
        glm::vec2 Size;
        bool TwoSided;
    };

    static AreaLightComponent to(const ReflType &v) noexcept // NOLINT
    {
        return {v.Radiance, v.Intensity, v.Size, v.TwoSided};
    }

    static ReflType from(const AreaLightComponent &v) noexcept // NOLINT
    {
        return {v.Radiance, v.Intensity, v.Size, v.TwoSided};
    }
};

} // namespace rfl