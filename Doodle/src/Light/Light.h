
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
    float Padding1; // 4 bytes to align to 16 bytes
    glm::vec3 Radiance{1.0f};
    float Intensity = 1.0f;
    float MinRange = 0.001f;
    float Range = 10.f;
    float Padding2[2];

    PointLight() = default;

    PointLight(glm::vec3 position, glm::vec3 radiance, float intensity, float minRange, float range)
        : Position(position), Radiance(radiance), Intensity(intensity), MinRange(minRange), Range(range)
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
    float Intensity = 1.0f;
    float MinRange = 0.001f;
    float Range = 10.f;
    float MinAngle = 0.01f;
    float Angle = 30.f;

    SpotLight() = default;

    SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 radiance, float intensity, float minRange, float range,
              float minAngle, float angle)
        : Position(position), Direction(direction), Radiance(radiance), Intensity(intensity), MinRange(minRange),
          Range(range), MinAngle(minAngle), Angle(angle)
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
    float ShadowBias = 0.001f;
    float ShadowNormalBias = 0.001f;
    float Padding1;
    glm::vec2 Resolution;
    float Padding2[2];
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
