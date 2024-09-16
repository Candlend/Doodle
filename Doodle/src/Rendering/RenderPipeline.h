#pragma once

#include "SceneRenderer.h"
#include "pch.h"

#include "Light.h"
#include "Singleton.h"
#include "UniformBuffer.h"

namespace Doodle
{

class RenderPass;
class RenderPassSpecification;
class DOO_API RenderPipeline : public Singleton<RenderPipeline>
{
public:
    RenderPipeline();

    void RegisterRenderPasses();

    void AddRenderPass(const std::string &name, std::shared_ptr<RenderPass> renderPass);

    template <typename T> void CreateRenderPass(const std::string &name, const RenderPassSpecification &specification);

    void RemoveRenderPass(const std::string &name);

    std::shared_ptr<RenderPass> GetRenderPass(const std::string &name);

    void BeginScene(Scene *scene);

    void EndScene();

    void Execute();

    std::shared_ptr<UniformBuffer> GetUniformBuffer(const std::string &name)
    {
        return m_uniformBuffers[name];
    }

    void AddUniformBuffer(const std::string &name, std::shared_ptr<UniformBuffer> uniformBuffer)
    {
        m_uniformBuffers[name] = uniformBuffer;
    }

    void RemoveUniformBuffer(const std::string &name)
    {
        m_uniformBuffers.erase(name);
    }

private:
    Scene *m_scene;
    std::unordered_map<std::string, std::shared_ptr<RenderPass>> m_renderPasses;
    std::unordered_map<std::string, std::shared_ptr<UniformBuffer>> m_uniformBuffers;
};

} // namespace Doodle