#pragma once

#include "EventManager.h"
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

struct UBOData
{
    UBOScene UBOScene;
    UBOPointLights UBOPointLights;
    UBOSpotLights UBOSpotLights;
};

class Scene;

class DOO_API SceneRenderer : public Singleton<SceneRenderer>,
                              public IEventHandler<ViewportResizeEvent, ExecutionOrder::First>,
                              public IEventHandler<SceneActivateEvent, ExecutionOrder::First>,
                              public IEventHandler<SceneDeactivateEvent, ExecutionOrder::Last>
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
    bool OnEvent(SceneActivateEvent &e) override;
    bool OnEvent(SceneDeactivateEvent &e) override;

    std::shared_ptr<FrameBuffer> m_frameBuffer;
};

} // namespace Doodle
