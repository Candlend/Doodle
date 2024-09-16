#pragma once

#include "Log.h"
#include "Scene.h"
#include "pch.h"
#include <entt/entt.hpp>
#include <memory>

#include "Camera.h"
#include "FrameBuffer.h"
#include "IEventHandler.h"
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

struct CameraData
{
    glm::mat4 View;
    glm::mat4 Projection;
    glm::mat4 ViewProjection;
    glm::vec3 Position;
};

struct SceneData
{
    glm::vec3 AmbientRadiance;
    CameraData CameraData;
    LightEnvironment LightEnvironment;
};

class Scene;

class DOO_API SceneRenderer : public Singleton<SceneRenderer>,
                              public IEventHandler<ViewportResizeEvent, ExecutionOrder::First>
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
    bool OnEvent(ViewportResizeEvent &e) override;

    std::shared_ptr<FrameBuffer> m_frameBuffer;

    std::shared_ptr<UniformBuffer> m_sceneUBO;
    std::shared_ptr<UniformBuffer> m_pointLightsUBO;
    std::shared_ptr<UniformBuffer> m_spotLightsUBO;

    void PrepareSceneData(std::shared_ptr<Scene> scene);

    SceneData m_sceneData;
};

} // namespace Doodle
