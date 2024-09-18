#pragma once

#include "Framebuffer.h"
#include "Material.h"
#include "RenderPipeline.h"
#include "Renderer.h"
#include "RendererAPI.h"
#include "Shader.h"
#include "glm/fwd.hpp"
#include "imgui.h"
#include "pch.h"

#include "Component.h"
#include "RenderPass.h"
#include <cstdint>
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
    friend class BloomPass;

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

    void SetMipCount(uint32_t mipCount)
    {
        if (mipCount == m_mipCount)
            return;
        m_mipChain.resize(m_mipCount);
        if (mipCount > m_mipCount)
        {
            for (unsigned int i = m_mipCount; i < mipCount; i++)
            {
                glm::u32vec2 mipSize = {static_cast<uint32_t>(m_width * std::pow(0.5f, i)),
                                        static_cast<uint32_t>(m_height * std::pow(0.5f, i))};

                FramebufferAttachmentSpecification attachments = {FramebufferTextureFormat::RGBA16F};
                FramebufferSpecification spec = {mipSize.x, mipSize.y, attachments};

                m_mipChain.push_back({FrameBuffer::Create(spec), mipSize});
            }
        }
        m_mipCount = mipCount;
    }

    uint32_t GetMipCount() const
    {
        return m_mipCount;
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
            Renderer::RenderFullscreenQuad(input, m_downsampleShader);
        }

        Renderer::SetBlend(BlendType::One, BlendType::One);
        m_upsampleShader->SetUniform1f("u_FilterRadius", m_blurRadius);
        // Upsample
        for (unsigned int i = m_mipCount - 1; i > 0; i--)
        {
            const BloomMip &mip = m_mipChain[i];
            const BloomMip &nextMip = m_mipChain[i - 1];

            nextMip.Fbo->Bind();
            Renderer::RenderFullscreenQuad(mip.Fbo, m_upsampleShader);
        }

        Renderer::SetBlend(BlendType::SrcAlpha, BlendType::OneMinusSrcAlpha);
        source->Bind();
        m_bloomShader->SetUniformTexture("u_BloomTexture", m_mipChain[0].Fbo->GetColorAttachmentTextureHandle());
        Renderer::RenderFullscreenQuad(source, m_bloomShader);
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
        m_bloomFbo->m_bloomShader->SetUniform1f("u_BloomStrength", m_bloomStrength);
        m_bloomFbo->m_bloomShader->SetUniform1f("u_Exposure", m_exposure);
        m_bloomFbo->RenderBloom(targetFrameBuffer);
    }

    void OnLayout() override
    {

        uint32_t mipCount = m_bloomFbo->GetMipCount();
        ImGui::DragInt("Mip Count", reinterpret_cast<int *>(&mipCount), 1, 1, 10);
        m_bloomFbo->SetMipCount(mipCount);

        ImGui::DragFloat("Blur Radius", &m_bloomFbo->m_blurRadius, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Bloom Strength", &m_bloomStrength, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Exposure", &m_exposure, 0.1f, 0.0f, 10.0f);
    }

private:
    float m_bloomStrength = 0.04f;
    float m_exposure = 1.0f;
    std::shared_ptr<BloomFbo> m_bloomFbo;
};

} // namespace Doodle