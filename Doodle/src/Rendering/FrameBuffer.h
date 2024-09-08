#pragma once

#include "pch.h"
#include <cstdint>
#include <memory>

namespace Doodle
{

enum class FramebufferTextureFormat
{
    None = 0,
    RGBA8,
    RED_INTEGER, // NOLINT
    DEPTH24STENCIL8,

    // Default
    Depth = DEPTH24STENCIL8
};

struct FramebufferTextureSpecification
{
    FramebufferTextureSpecification() = default;
    FramebufferTextureSpecification(FramebufferTextureFormat format) : TextureFormat(format)
    {
    }

    FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
    // TODO: filtering/wrap
};

struct FramebufferAttachmentSpecification
{
    FramebufferAttachmentSpecification() = default;
    FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
        : Attachments(attachments)
    {
    }

    std::vector<FramebufferTextureSpecification> Attachments;
};

struct FramebufferSpecification
{
    uint32_t Width = 0, Height = 0;
    FramebufferAttachmentSpecification Attachments;
    uint32_t Samples = 1;

    bool SwapChainTarget = false; // TODO 未使用
};

class FrameBuffer
{
public:
    static std::shared_ptr<FrameBuffer> Create(const FramebufferSpecification &specification);
    virtual ~FrameBuffer() = default;

    virtual void Bind() = 0;
    virtual void Unbind() = 0;
    virtual void Resize(uint32_t width, uint32_t height) = 0;

    virtual uint32_t GetColorAttachmentRendererID(size_t index) const = 0;
    virtual uint32_t GetDepthAttachmentRendererID() const = 0;
    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;

    virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;
    virtual FramebufferSpecification &GetSpecification() = 0;
};

} // namespace Doodle