#pragma once

#include "Log.h"
#include "pch.h"
#include <entt/entt.hpp>
#include <memory>

#include "Camera.h"
#include "FrameBuffer.h"
#include "Light.h"
#include "SceneEvent.h"
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

class DOO_API SceneRenderer : public Singleton<SceneRenderer>
{
public:
    SceneRenderer();
    ~SceneRenderer();
    void Render();
    std::shared_ptr<FrameBuffer> GetFrameBuffer() const
    {
        return m_frameBuffer;
    }

private:
    bool OnViewportResize(const ViewportResizeEvent &e);
    std::shared_ptr<UniformBuffer> m_sceneUBO;
    std::shared_ptr<UniformBuffer> m_pointLightsUBO;
    std::shared_ptr<UniformBuffer> m_spotLightsUBO;
    LightEnvironment m_lightEnvironment;
    std::shared_ptr<FrameBuffer> m_frameBuffer;
};

} // namespace Doodle
