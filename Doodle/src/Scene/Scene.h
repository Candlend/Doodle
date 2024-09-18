#pragma once

#include "Texture.h"
#include "entt/entity/fwd.hpp"
#include "pch.h"
#include <entt/entt.hpp>

#include "ApplicationEvent.h"
#include "Camera.h"
#include "EventManager.h"
#include "Light.h"
#include "UUID.h"
#include "UniformBuffer.h"

namespace Doodle
{

struct LightData
{
    static constexpr size_t MAX_DIRECTIONAL_LIGHTS = 4;

    DirectionalLight DirectionalLights[MAX_DIRECTIONAL_LIGHTS];
    std::vector<PointLight> PointLights;
    std::vector<SpotLight> SpotLights;
    std::vector<AreaLight> AreaLights;

    [[nodiscard]] uint32_t GetPointLightsSize() const
    {
        return static_cast<uint32_t>(PointLights.size() * sizeof(PointLight));
    }

    [[nodiscard]] uint32_t GetSpotLightsSize() const
    {
        return static_cast<uint32_t>(SpotLights.size() * sizeof(SpotLight));
    }

    [[nodiscard]] uint32_t GetAreaLightsSize() const
    {
        return static_cast<uint32_t>(AreaLights.size() * sizeof(AreaLight));
    }
};

struct CameraData
{
    glm::mat4 View;
    glm::mat4 Projection;
    glm::mat4 ViewProjection;
    glm::vec3 Position;
    float Near;
    float Far;
};

struct EnvironmentData
{
    std::shared_ptr<TextureCube> RadianceMap;
    std::shared_ptr<TextureCube> IrradianceMap;
    float Intensity = 1.0f;
    float Rotation = 0.0f;
};

struct SceneData
{
    CameraData CameraData;
    EnvironmentData EnvironmentData;
    LightData LightData;
    float ShadowBias = 0.001f;
    float ShadowNormalBias = 0.001f;
};

class Entity;
class BaseComponent;
class SceneRenderer;
class DOO_API Scene : public std::enable_shared_from_this<Scene>
{
    friend class SceneRenderer;
    friend class Entity;

public:
    static std::shared_ptr<Scene> Create(const std::string &name);
    Scene(const std::string &name);
    ~Scene();
    Entity GetMainCameraEntity();

    Entity CreateEntity(const std::string &name);
    Entity FindEntity(const std::string &name) const;
    Entity GetEntity(const UUID &id) const;
    std::vector<Entity> GetEntities() const;
    std::vector<BaseComponent *> GetComponents(const UUID &id) const;
    void AddEntity(const Entity &entity);
    void RemoveEntity(const UUID &id);
    void DestroyEntity(const Entity &entity);
    void LoadEnvironment(const std::string &filepath);

    inline std::string GetName() const
    {
        return m_name;
    }

    inline bool IsActive() const
    {
        return m_active;
    }

    bool operator==(const Scene &other) const
    {
        return m_name == other.m_name;
    }

    void BeginScene();
    void EndScene();

    template <typename... Args> auto View()
    {
        return m_registry.view<Args...>();
    }

    SceneData &GetData()
    {
        return m_sceneData;
    }

    void SetupSceneData();

private:
    std::string m_name;
    bool m_active = false;
    std::unordered_map<UUID, Entity> m_entityMap;
    std::unordered_map<UUID, std::vector<BaseComponent *>> m_entityComponents;
    entt::registry m_registry;

    SceneData m_sceneData;
};

} // namespace Doodle
