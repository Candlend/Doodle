#pragma once

#include "pch.h"
#include <glm/glm.hpp>

#include "BaseComponent.h"

namespace Doodle
{

struct DirectionalLightComponent : public BaseComponent
{
    glm::vec3 Radiance{1.0f};
    float Intensity = 0.0f;
};

struct PointLightComponent : public BaseComponent
{
    glm::vec3 Radiance{1.0f};
    float Intensity = 0.0f;
    float MinRadius = 0.001f;
    float Radius = 25.0f;
    float Falloff = 1.f;
    float SourceSize = 0.1f;
};

struct SpotLightComponent : public BaseComponent
{
    glm::vec3 Radiance{1.0f};
    float Intensity = 0.0f;
    float AngleAttenuation = 0.0f;
    float Range = 0.1f;
    float Angle = 0.0f;
    float Falloff = 1.0f;
};

} // namespace Doodle