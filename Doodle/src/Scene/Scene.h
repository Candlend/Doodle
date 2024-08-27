#pragma once

#include "pch.h"
#include <entt/entt.hpp>

#include "Camera.h"
#include "Component.h"
#include "Entity.h"

namespace Doodle
{

struct DirectionalLight
{
    glm::vec3 Direction{0.0f};
    glm::vec3 Radiance{1.0f};
    float Intensity = 0.0f;
};

struct PointLight
{
    glm::vec3 Position{0.0f};
    glm::vec3 Radiance{1.0f};
    float Intensity = 0.0f;
    float MinRadius = 0.001f;
    float Radius = 25.0f;
    float Falloff = 1.f;
    float SourceSize = 0.1f;
};

struct SpotLight
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

class DOO_API Scene : public std::enable_shared_from_this<Scene>
{
public:
    static std::shared_ptr<Scene> Create();
    std::shared_ptr<Entity> CreateEntity(const std::string &name);
    std::shared_ptr<Entity> GetEntity(const std::string &name) const;
    void Render();
    std::shared_ptr<Entity> GetMainCameraEntity();
    void Begin();
    void End();

private:
    LightEnvironment m_lightEnvironment;
    entt::registry m_registry;
    std::unordered_map<std::string, std::shared_ptr<Entity>> m_entities;
};

} // namespace Doodle
