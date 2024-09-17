
#pragma once

#include "Log.h"
#include "glm/fwd.hpp"
#include "pch.h"
#include <glm/glm.hpp>

struct DirectionalLight
{
    glm::vec3 Direction{0.0f}; // 12 bytes
    float Padding;      // 4 bytes to align to 16 bytes
    glm::vec3 Radiance{1.0f}; // 12 bytes
    float Intensity = 0.0f;   // 4 bytes

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

struct AreaLight
{
    glm::vec3 Points1;
    float Padding1;
    glm::vec3 Points2;
    float Padding2;
    glm::vec3 Points3;
    float Padding3;
    glm::vec3 Points4;
    float Padding4;
    glm::vec3 Radiance{1.0f};
    float Intensity = 0.0f;
    int TwoSided = 0;
    float Padding5[3];

    AreaLight() = default;

    AreaLight(glm::vec3 points[4], glm::vec3 radiance, float intensity, bool twoSided)
        : Radiance(radiance), Intensity(intensity), TwoSided(twoSided)
    {
        Points1 = points[0];
        Points2 = points[1];
        Points3 = points[2];
        Points4 = points[3];
    }
};

struct UBOScene
{
    DirectionalLight DirectionalLights[4];
    glm::vec3 CameraPosition{0.0f};
    float EnvironmentIntensity = 1.0f;
    float EnvironmentRotation = 0.0f;
    float Padding2[3];
};

struct UBOPointLights
{
    uint32_t Count;
    float Padding[3];
    PointLight PointLights[256];
};

struct UBOSpotLights
{
    uint32_t Count;
    float Padding[3];
    SpotLight SpotLights[256];
};

struct UBOAreaLights
{
    uint32_t Count;
    float Padding[3];
    AreaLight AreaLights[32];
};
