#pragma once

#include "pch.h"
#include <variant>

#include "Framebuffer.h"
#include "RenderPipeline.h"
#include "Scene.h"

namespace Doodle
{

struct RenderPassSpecification
{
    std::shared_ptr<FrameBuffer> TargetFrameBuffer;
};

using RenderPassInput = typename std::variant<std::shared_ptr<FrameBuffer>>;

class DOO_API RenderPass
{
    friend class RenderPipeline;

public:
    RenderPass(const RenderPassSpecification &specification) : m_specification(specification)
    {
    }

    virtual ~RenderPass() = default;

    RenderPassSpecification &GetSpecification()
    {
        return m_specification;
    }
    const RenderPassSpecification &GetSpecification() const
    {
        return m_specification;
    }

    virtual void BeginScene() = 0;
    virtual void EndScene() = 0;
    virtual void Execute() = 0;

    template <typename T> std::shared_ptr<T> GetInput(const std::string &name)
    {
        return std::get<std::shared_ptr<T>>(m_inputs[name]);
    }

    template <typename T> void SetInput(const std::string &name, std::shared_ptr<T> input)
    {
        m_inputs[name] = input;
    }

protected:
    RenderPassSpecification m_specification;
    std::unordered_map<std::string, RenderPassInput> m_inputs;
    Scene *m_scene = nullptr;
};

} // namespace Doodle