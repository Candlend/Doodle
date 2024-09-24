#pragma once

#include "pch.h"
#include <imgui.h>

#include "Framebuffer.h"
#include "RenderPipeline.h"
#include "Scene.h"

namespace Doodle
{

struct RenderPassSpecification
{
    std::string DebugName;
    std::shared_ptr<FrameBuffer> TargetFrameBuffer;
};

class DOO_API RenderPass
{
    friend class RenderPipeline;

public:
    RenderPass(const RenderPassSpecification &specification) : m_specification(specification)
    {
    }

    virtual ~RenderPass() = default;

    const std::string &GetName() const
    {
        return m_specification.DebugName;
    }

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

    virtual void OnLayout()
    {
        ImGui::TextDisabled("No layout available");
    }

protected:
    RenderPassSpecification m_specification;
    Scene *m_scene = nullptr;
};

} // namespace Doodle