
#pragma once

#include "pch.h"
#include <glm/glm.hpp>

struct alignas(16) DirectionalLight
{
    glm::vec3 Direction{0.0f};
    glm::vec3 Radiance{1.0f};
    float Intensity = 0.0f;
};

struct alignas(16) PointLight
{
    glm::vec3 Position{0.0f};
    glm::vec3 Radiance{1.0f};
    float Intensity = 0.0f;
    float MinRadius = 0.001f;
    float Radius = 25.0f;
    float Falloff = 1.f;
    float SourceSize = 0.1f;
};

struct alignas(16) SpotLight
{
    glm::vec3 Position{0.0f};
    glm::vec3 Direction{0.0f};
    glm::vec3 Radiance{1.0f};
    float Intensity = 0.0f;
    float AngleAttenuation = 0.0f;
    float Range = 0.1f;
    float Angle = 0.0f;
    float Falloff = 1.0f;
};

struct UBOScene
{
    DirectionalLight DirectionalLight;
};

struct UBOPointLights
{
    uint32_t Count;
    PointLight PointLights[512];
};

struct UBOSpotLights
{
    uint32_t Count;
    SpotLight SpotLights[512];
};

struct LightEnvironment
{
    static constexpr size_t MAX_DIRECTIONAL_LIGHTS = 4;

    DirectionalLight DirectionalLights[MAX_DIRECTIONAL_LIGHTS];
    std::vector<PointLight> PointLights;
    std::vector<SpotLight> SpotLights;
    [[nodiscard]] uint32_t GetPointLightsSize() const
    {
        return static_cast<uint32_t>(PointLights.size() * sizeof(PointLight));
    }
    [[nodiscard]] uint32_t GetSpotLightsSize() const
    {
        return static_cast<uint32_t>(SpotLights.size() * sizeof(SpotLight));
    }
};