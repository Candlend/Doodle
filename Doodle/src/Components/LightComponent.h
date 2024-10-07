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

    void OnInspectorLayout() override;

    void OnDrawGizmos(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) override;

    rfl::Generic::Object SerializeToObject() const override;

    void DeserializeFromObject(const rfl::Generic::Object &object) override;
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

    void OnInspectorLayout() override;

    void OnDrawGizmos(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) override;

    rfl::Generic::Object SerializeToObject() const override;

    void DeserializeFromObject(const rfl::Generic::Object &object) override;
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

    void OnInspectorLayout() override;

    void OnDrawGizmos(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) override;

    rfl::Generic::Object SerializeToObject() const override;

    void DeserializeFromObject(const rfl::Generic::Object &object) override;
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

    void OnInspectorLayout() override;

    void OnDrawGizmos(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) override;

    rfl::Generic::Object SerializeToObject() const override;

    void DeserializeFromObject(const rfl::Generic::Object &object) override;
};

} // namespace Doodle
