#include <glad/glad.h>

#include "Framebuffer.h"
#include "Log.h"
#include "Renderer.h"

namespace Doodle
{

static const uint32_t MAX_FRAMEBUFFER_SIZE = 8192;

static GLenum TextureTarget(bool multisampled)
{
    return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

static void CreateTextures(bool multisampled, uint32_t *outID, uint32_t count)
{
    glCreateTextures(TextureTarget(multisampled), count, outID);
}

static void BindTexture(bool multisampled, uint32_t id)
{
    glBindTexture(TextureTarget(multisampled), id);
}

static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width,
                               uint32_t height, int index)
{
    bool multisampled = samples > 1;
    if (multisampled)
    {
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
}

static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width,
                               uint32_t height)
{
    bool multisampled = samples > 1;
    if (multisampled)
    {
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
    }
    else
    {
        glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
}

static bool IsDepthFormat(FramebufferTextureFormat format)
{
    return format == FramebufferTextureFormat::DEPTH24STENCIL8;
}

static GLenum GetGLFormat(FramebufferTextureFormat format)
{
    switch (format)
    {
    case FramebufferTextureFormat::RGBA8:
        return GL_RGBA8;
    case FramebufferTextureFormat::RED_INTEGER:
        return GL_R32I;
    default:
        DOO_CORE_ASSERT(false);
        return 0;
    }
}

class OpenGLFramebuffer : public FrameBuffer
{
public:
    OpenGLFramebuffer(const FramebufferSpecification &spec) : m_specification(spec)
    {
        for (const auto &attachment : m_specification.Attachments.Attachments)
        {
            if (!IsDepthFormat(attachment.TextureFormat))
                m_colorAttachmentSpecifications.push_back(attachment);
            else
                m_depthAttachmentSpecification = attachment;
        }
        Renderer::Submit([this]() { Invalidate(); });
    }
    ~OpenGLFramebuffer()
    {
        Renderer::Submit([this]() {
            glDeleteFramebuffers(1, &m_rendererId);
            glDeleteTextures(m_colorAttachments.size(), m_colorAttachments.data());
            if (m_depthAttachment)
                glDeleteTextures(1, &m_depthAttachment);
        });
    }

    void Bind() override
    {
        Renderer::Submit([this]() {
            glBindFramebuffer(GL_FRAMEBUFFER, m_rendererId);
            glViewport(0, 0, m_specification.Width, m_specification.Height);
        });
    }
    void Unbind() override
    {
        Renderer::Submit([]() { glBindFramebuffer(GL_FRAMEBUFFER, 0); });
    }
    void Resize(uint32_t width, uint32_t height) override
    {
        if (width == 0 || height == 0 || width > MAX_FRAMEBUFFER_SIZE || height > MAX_FRAMEBUFFER_SIZE)
        {
            DOO_CORE_WARN("Attempted to resize framebuffer to {0}, {1}", width, height);
            return;
        }
        m_specification.Width = width;
        m_specification.Height = height;

        Renderer::Submit([this]() { Invalidate(); });
    }

    uint32_t GetColorAttachmentRendererID(size_t index) const override
    {
        DOO_CORE_ASSERT(index < m_colorAttachments.size());
        return m_colorAttachments[index];
    }
    uint32_t GetDepthAttachmentRendererID() const override
    {
        return m_depthAttachment;
    }
    uint32_t GetWidth() const override
    {
        return m_specification.Width;
    }
    uint32_t GetHeight() const override
    {
        return m_specification.Height;
    }

    void ClearAttachment(uint32_t attachmentIndex, int value) override
    {
        DOO_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

        Renderer::Submit([this, attachmentIndex, value]() {
            auto &spec = m_colorAttachmentSpecifications[attachmentIndex];
            glClearTexImage(m_colorAttachments[attachmentIndex], 0, GetGLFormat(spec.TextureFormat), GL_INT, &value);
        });
    }
    FramebufferSpecification &GetSpecification() override
    {
        return m_specification;
    }

private:
    void Invalidate()
    {
        if (m_rendererId)
        {
            glDeleteFramebuffers(1, &m_rendererId);
            glDeleteTextures(m_colorAttachments.size(), m_colorAttachments.data());
            if (m_depthAttachment)
                glDeleteTextures(1, &m_depthAttachment);

            m_colorAttachments.clear();
            m_depthAttachment = 0;
        }

        glCreateFramebuffers(1, &m_rendererId);
        glBindFramebuffer(GL_FRAMEBUFFER, m_rendererId);

        bool multisample = m_specification.Samples > 1;

        // Attachments
        if (!m_colorAttachmentSpecifications.empty())
        {
            m_colorAttachments.resize(m_colorAttachmentSpecifications.size());
            CreateTextures(multisample, m_colorAttachments.data(), m_colorAttachments.size());

            for (size_t i = 0; i < m_colorAttachments.size(); i++)
            {
                BindTexture(multisample, m_colorAttachments[i]);
                switch (m_colorAttachmentSpecifications[i].TextureFormat)
                {
                case FramebufferTextureFormat::RGBA8:
                    AttachColorTexture(m_colorAttachments[i], m_specification.Samples, GL_RGBA8, GL_RGBA,
                                       m_specification.Width, m_specification.Height, i);
                    break;
                case FramebufferTextureFormat::RED_INTEGER:
                    AttachColorTexture(m_colorAttachments[i], m_specification.Samples, GL_R32I, GL_RED_INTEGER,
                                       m_specification.Width, m_specification.Height, i);
                    break;
                case FramebufferTextureFormat::DEPTH24STENCIL8:
                case FramebufferTextureFormat::None:
                    break;
                }
            }
        }

        if (m_depthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
        {
            CreateTextures(multisample, &m_depthAttachment, 1);
            BindTexture(multisample, m_depthAttachment);
            if (m_depthAttachmentSpecification.TextureFormat == FramebufferTextureFormat::DEPTH24STENCIL8)
            {
                AttachDepthTexture(m_depthAttachment, m_specification.Samples, GL_DEPTH24_STENCIL8,
                                   GL_DEPTH_STENCIL_ATTACHMENT, m_specification.Width, m_specification.Height);
            }
        }

        if (m_colorAttachments.size() > 1)
        {
            DOO_CORE_ASSERT(m_ColorAttachments.size() <= 4);
            GLenum buffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
                                 GL_COLOR_ATTACHMENT3};
            glDrawBuffers(static_cast<GLsizei>(m_colorAttachments.size()), buffers);
        }
        else if (m_colorAttachments.empty())
        {
            glDrawBuffer(GL_NONE);
        }

        DOO_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
                        "Framebuffer is incomplete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    uint32_t m_rendererId = 0;
    std::vector<uint32_t> m_colorAttachments;
    uint32_t m_depthAttachment = 0;
    FramebufferSpecification m_specification;
    std::vector<FramebufferTextureSpecification> m_colorAttachmentSpecifications;
    FramebufferTextureSpecification m_depthAttachmentSpecification;
};

std::shared_ptr<FrameBuffer> FrameBuffer::Create(const FramebufferSpecification &specification)
{
    return std::make_shared<OpenGLFramebuffer>(specification);
}

} // namespace Doodle
