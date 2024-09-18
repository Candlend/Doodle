#pragma once

#include "RendererAPI.h"
#include "pch.h"

#include "Shader.h"
#include <cstddef>
#include <cstdint>
#include <memory>

namespace Doodle
{

enum class FramebufferTextureFormat
{
    None = 0,
    RGBA8,
    RGBA16F,
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
    FramebufferAttachmentSpecification(const std::initializer_list<FramebufferTextureSpecification> &attachments)
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

    virtual uint32_t GetRendererID() const = 0;
    virtual uint32_t GetColorAttachmentRendererID(size_t index) const = 0;
    uint32_t GetColorAttachmentRendererID() const
    {
        return GetColorAttachmentRendererID(0);
    }
    virtual uint64_t GetColorAttachmentTextureHandle(size_t index) const = 0;
    uint64_t GetColorAttachmentTextureHandle() const
    {
        return GetColorAttachmentTextureHandle(0);
    }
    virtual uint32_t GetDepthAttachmentRendererID() const = 0;
    virtual uint64_t GetDepthAttachmentTextureHandle() const = 0;
    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;
    virtual size_t GetColorAttachmentCount() const = 0;

    virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;
    virtual FramebufferSpecification &GetSpecification() = 0;

    virtual void BlitTo(std::shared_ptr<FrameBuffer> target, BufferFlags bufferFlags) = 0;
    virtual void BlitTo(std::shared_ptr<FrameBuffer> target)
    {
        BlitTo(target, BufferFlags::All);
    }
};

} // namespace Doodle
