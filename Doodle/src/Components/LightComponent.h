#pragma once

#include "pch.h"
#include <glm/glm.hpp>

#include "BaseComponent.h"

namespace Doodle
{

struct DirectionalLightComponent : public BaseComponent
{
    COMPONENT_CLASS_TYPE(DirectionalLight)

    glm::vec3 Radiance{1.0f};
    float Intensity = 0.0f;

    void OnInspectorLayout() override
    {
        ImGui::ColorEdit3("Radiance", glm::value_ptr(Radiance));
        ImGui::DragFloat("Intensity", &Intensity, 0.1f, 0.0f, 100.0f);
    }
};

struct PointLightComponent : public BaseComponent
{
    COMPONENT_CLASS_TYPE(PointLight)

    glm::vec3 Radiance{1.0f};
    float Intensity = 0.0f;
    float MinRadius = 0.001f;
    float Radius = 25.0f;
    float Falloff = 1.f;
    float SourceSize = 0.1f;

    void OnInspectorLayout() override
    {
        ImGui::ColorEdit3("Radiance", glm::value_ptr(Radiance));
        ImGui::DragFloat("Intensity", &Intensity, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("Min Radius", &MinRadius, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("Radius", &Radius, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("Falloff", &Falloff, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("Source Size", &SourceSize, 0.1f, 0.0f, 100.0f);
    }
};

struct SpotLightComponent : public BaseComponent
{
    COMPONENT_CLASS_TYPE(SpotLight)

    glm::vec3 Radiance{1.0f};
    float Intensity = 0.0f;
    float AngleAttenuation = 0.0f;
    float Range = 0.1f;
    float Angle = 0.0f;
    float Falloff = 1.0f;

    void OnInspectorLayout() override
    {
        ImGui::ColorEdit3("Radiance", glm::value_ptr(Radiance));
        ImGui::DragFloat("Intensity", &Intensity, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("Angle Attenuation", &AngleAttenuation, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("Range", &Range, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("Angle", &Angle, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("Falloff", &Falloff, 0.1f, 0.0f, 100.0f);
    }
};

} // namespace Doodle