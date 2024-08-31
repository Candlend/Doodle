
#pragma once

#include "Log.h"
#include "pch.h"
#include <glm/glm.hpp>

struct DirectionalLight
{
    glm::vec3 Direction; // 12 bytes
    float Padding;      // 4 bytes to align to 16 bytes
    glm::vec3 Radiance;  // 12 bytes
    float Intensity;     // 4 bytes

    DirectionalLight() = default;

    DirectionalLight(glm::vec3 direction, glm::vec3 radiance, float intensity)
        : Direction(direction), Radiance(radiance), Intensity(intensity)
    {
    }
};

struct PointLight
{
    glm::vec3 Position{0.0f};
    float Padding; // 4 bytes to align to 16 bytes
    glm::vec3 Radiance{1.0f};
    float Intensity = 0.0f;
    float MinRadius = 0.001f;
    float Radius = 25.0f;
    float Falloff = 1.f;
    float SourceSize = 0.1f;

    PointLight() = default;

    PointLight(glm::vec3 position, glm::vec3 radiance, float intensity, float minRadius, float radius, float falloff,
               float sourceSize)
        : Position(position), Radiance(radiance), Intensity(intensity), MinRadius(minRadius), Radius(radius),
          Falloff(falloff), SourceSize(sourceSize)
    {
    }
};

struct SpotLight
{
    glm::vec3 Position{0.0f};
    float Padding1; // 4 bytes to align to 16 bytes
    glm::vec3 Direction{0.0f};
    float Padding2; // 4 bytes to align to 16 bytes
    glm::vec3 Radiance{1.0f};
    float Intensity = 0.0f;
    float AngleAttenuation = 0.0f;
    float Range = 0.1f;
    float Angle = 0.0f;
    float Falloff = 1.0f;

    SpotLight() = default;

    SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 radiance, float intensity, float angleAttenuation,
              float range, float angle, float falloff)
        : Position(position), Direction(direction), Radiance(radiance), Intensity(intensity),
          AngleAttenuation(angleAttenuation), Range(range), Angle(angle), Falloff(falloff)
    {
    }
};

struct UBOScene
{
    DirectionalLight DirectionalLight;

    ~UBOScene()
    {
        DOO_CORE_TRACE("UBOScene destroyed");
    }
};

struct UBOPointLights
{
    uint32_t Count;
    float Padding[3];
    PointLight PointLights[512];
};

struct UBOSpotLights
{
    uint32_t Count;
    float Padding[3];
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