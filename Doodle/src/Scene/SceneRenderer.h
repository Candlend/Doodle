#pragma once

#include "Log.h"
#include "pch.h"
#include <entt/entt.hpp>

#include "Camera.h"
#include "FrameBuffer.h"
#include "Light.h"
#include "UniformBuffer.h"

namespace Doodle
{

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

class Scene;

class DOO_API SceneRenderer // for each scene
{
public:
    SceneRenderer(Scene *scene);
    ~SceneRenderer();
    void Render();

private:
    Scene *m_scene;
    entt::registry &m_registry;
    std::shared_ptr<UniformBuffer> m_sceneUBO;
    std::shared_ptr<UniformBuffer> m_pointLightsUBO;
    std::shared_ptr<UniformBuffer> m_spotLightsUBO;
    LightEnvironment m_lightEnvironment;
    std::shared_ptr<FrameBuffer> m_frameBuffer;
};

} // namespace Doodle
