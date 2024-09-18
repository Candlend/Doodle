#pragma once

#include "Framebuffer.h"
#include "Material.h"
#include "RenderPipeline.h"
#include "Renderer.h"
#include "RendererAPI.h"
#include "Shader.h"
#include "glm/fwd.hpp"
#include "pch.h"

#include "Component.h"
#include "RenderPass.h"
#include <memory>

namespace Doodle
{

struct BloomMip
{
    std::shared_ptr<FrameBuffer> Fbo = nullptr;
    glm::u32vec2 Size; // 纹理的尺寸
};

class DOO_API BloomFbo
{
public:
    BloomFbo(uint32_t width, uint32_t height, uint32_t mipCount)
        : m_width(width), m_height(height), m_mipCount(mipCount)
    {
        m_downsampleShader = ShaderLibrary::Get()->GetShader("downsample");
        m_upsampleShader = ShaderLibrary::Get()->GetShader("upsample");
        m_bloomShader = ShaderLibrary::Get()->GetShader("bloom");

        m_mipChain.resize(m_mipCount);

        // 创建多个 mip 级别的纹理
        for (unsigned int i = 0; i < m_mipCount; i++)
        {
            glm::u32vec2 mipSize = {static_cast<uint32_t>(width * std::pow(0.5f, i)),
                                    static_cast<uint32_t>(height * std::pow(0.5f, i))};

            FramebufferAttachmentSpecification attachments = {FramebufferTextureFormat::RGBA16F};
            FramebufferSpecification spec = {mipSize.x, mipSize.y, attachments};

            m_mipChain[i] = {FrameBuffer::Create(spec), mipSize};
        }

        RenderPipeline::Get()->SetFrameBuffer("BloomMap", m_mipChain[0].Fbo);
    }

    void SetBlurRadius(float value)
    {
        m_blurRadius = value;
    }

    void SetMipCount(uint32_t mipCount)
    {
        if (mipCount == m_mipCount)
            return;
        if (mipCount > m_mipCount)
        {
            Resize(m_width, m_height);
        }
        m_mipCount = mipCount;
        m_mipChain.resize(m_mipCount);
    }

    void Resize(uint32_t width, uint32_t height)
    {
        m_width = width;
        m_height = height;

        // 创建多个 mip 级别的纹理
        for (unsigned int i = 0; i < m_mipCount; i++)
        {
            glm::u32vec2 mipSize = {static_cast<uint32_t>(width * std::pow(0.5f, i)),
                                    static_cast<uint32_t>(height * std::pow(0.5f, i))};

            FramebufferAttachmentSpecification attachments = {FramebufferTextureFormat::RGBA16F};
            FramebufferSpecification spec = {mipSize.x, mipSize.y, attachments};

            m_mipChain[i].Fbo->Resize(mipSize.x, mipSize.y);
            m_mipChain[i].Size = mipSize;
        }
    }

    void RenderBloom(std::shared_ptr<FrameBuffer> source)
    {
        if (m_mipCount == 0)
            return;
        // Downsample
        for (unsigned int i = 0; i < m_mipCount; i++)
        {
            const BloomMip &mip = m_mipChain[i];
            mip.Fbo->Bind();
            auto input = (i == 0) ? source : m_mipChain[i - 1].Fbo;
            Renderer::RenderFullscreenQuad(input, 0, m_downsampleShader);
        }

        Renderer::SetBlend(BlendType::One, BlendType::One);
        m_upsampleShader->SetUniform1f("u_FilterRadius", m_blurRadius);
        // Upsample
        for (unsigned int i = m_mipCount - 1; i > 0; i--)
        {
            const BloomMip &mip = m_mipChain[i];
            const BloomMip &nextMip = m_mipChain[i - 1];

            nextMip.Fbo->Bind();
            Renderer::RenderFullscreenQuad(mip.Fbo, 0, m_upsampleShader);
        }

        Renderer::SetBlend(BlendType::SrcAlpha, BlendType::OneMinusSrcAlpha);
        source->Bind();
        m_bloomShader->SetUniformTexture("u_BloomTexture", m_mipChain[0].Fbo->GetColorAttachmentTextureHandle());
        Renderer::RenderFullscreenQuad(source, 0, m_bloomShader);
        // 返回默认的混合模式
    }

private:
    unsigned int m_width, m_height;
    unsigned int m_mipCount;
    std::vector<BloomMip> m_mipChain;
    std::shared_ptr<Shader> m_downsampleShader;
    std::shared_ptr<Shader> m_upsampleShader;
    std::shared_ptr<Shader> m_bloomShader;
    float m_blurRadius = 0.005f;
};

class DOO_API BloomPass : public RenderPass
{
public:
    BloomPass(const RenderPassSpecification &specification) : RenderPass(specification)
    {

        m_bloomFbo = std::make_shared<BloomFbo>(1920, 1080, 5);
    }

    void BeginScene() override
    {
    }

    void EndScene() override
    {
    }

    void Execute() override
    {
        auto targetFrameBuffer = GetSpecification().TargetFrameBuffer;
        auto width = targetFrameBuffer->GetWidth();
        auto height = targetFrameBuffer->GetHeight();
        m_bloomFbo->Resize(width, height);
        m_bloomFbo->SetBlurRadius(0.005f);
        m_bloomFbo->SetMipCount(5);

        m_bloomFbo->RenderBloom(targetFrameBuffer);
    }

private:
    void Downsample()
    {
        // print 1
    }

    std::shared_ptr<BloomFbo> m_bloomFbo;
};

} // namespace Doodle