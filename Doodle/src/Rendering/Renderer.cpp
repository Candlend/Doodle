#include "pch.h"
#include <glad/glad.h>

#include "ApplicationEvent.h"
#include "EventManager.h"
#include "FrameBuffer.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Shader.h"
#include "ShaderLibrary.h"

namespace Doodle
{

void Renderer::Initialize()
{
    Renderer::Submit([]() { RendererAPI::Initialize(); });

    EventManager::Get()->AddListener<AppRenderEvent>(this, &Renderer::BeginFrame, ExecutionOrder::First);
    EventManager::Get()->AddListener<AppRenderEvent>(this, &Renderer::EndFrame, ExecutionOrder::Last);
}

void Renderer::Deinitialize()
{
    EventManager::Get()->RemoveListener<AppRenderEvent>(this, &Renderer::BeginFrame);
    EventManager::Get()->RemoveListener<AppRenderEvent>(this, &Renderer::EndFrame);
}

void Renderer::BeginFrame()
{
}

void Renderer::EndFrame()
{
    Renderer::WaitAndRender();
}

void Renderer::Clear(BufferFlags bufferFlags)
{
    Renderer::Submit([bufferFlags]() { RendererAPI::Clear(bufferFlags); });
}

void Renderer::SetClearColor(float r, float g, float b, float a)
{
    Renderer::Submit([r, g, b, a]() { RendererAPI::SetClearColor(r, g, b, a); });
}

void Renderer::DrawIndexed(unsigned int count)
{
    Renderer::Submit([count]() { RendererAPI::DrawIndexed(count); });
}

void Renderer::Draw(unsigned int count, PrimitiveType type)
{
    Renderer::Submit([count, type]() { RendererAPI::Draw(count, type); });
}

void Renderer::RenderFullscreenQuad(std::shared_ptr<Texture> texture, std::shared_ptr<Shader> shader)
{
    Renderer::SetDepthTest(DepthTestType::Disabled);
    if (!shader)
        shader = ShaderLibrary::Get()->GetShader("image");
    shader->Bind();
    texture->Bind();
    Mesh::GetQuad()->Render();
    Renderer::SetDepthTest(DepthTestType::Less);
    texture->Unbind();
    shader->Unbind();
}

void Renderer::RenderFullscreenQuad(uint32_t textureID, std::shared_ptr<Shader> shader)
{
    Renderer::SetDepthTest(DepthTestType::Disabled);
    if (!shader)
        shader = ShaderLibrary::Get()->GetShader("image");
    shader->Bind();
    Renderer::Submit([textureID]() { glBindTextureUnit(0, textureID); });
    Mesh::GetQuad()->Render();
    Renderer::SetDepthTest(DepthTestType::Less);
    Renderer::Submit([textureID]() { glBindTextureUnit(0, 0); });
}

void Renderer::RenderFullscreenQuad(std::shared_ptr<FrameBuffer> framebuffer, std::shared_ptr<Shader> shader)
{
    Renderer::SetDepthTest(DepthTestType::Disabled);
    if (!shader)
        shader = ShaderLibrary::Get()->GetShader("image");
    shader->Bind();
    Renderer::Submit([framebuffer]() {
        for (size_t i = 0; i < framebuffer->GetColorAttachmentCount(); i++)
        {
            glBindTextureUnit(i, framebuffer->GetColorAttachmentRendererID(i));
        }
    });
    Mesh::GetQuad()->Render();
    Renderer::SetDepthTest(DepthTestType::Less);
    shader->Unbind();
}

void Renderer::SetDepthTest(DepthTestType type)
{
    Renderer::Submit([type]() { RendererAPI::SetDepthTest(type); });
}

void Renderer::SetDepthWrite(bool write)
{
    Renderer::Submit([write]() { RendererAPI::SetDepthWrite(write); });
}

void Renderer::SetCullFace(CullFaceType type)
{
    Renderer::Submit([type]() { RendererAPI::SetCullFace(type); });
}

void Renderer::SetBlend(BlendType src, BlendType dst)
{
    Renderer::Submit([src, dst]() { RendererAPI::SetBlend(src, dst); });
}

void Renderer::SetBlendEquation(BlendEquationType type)
{
    Renderer::Submit([type]() { RendererAPI::SetBlendEquation(type); });
}

void Renderer::SetPolygonMode(PolygonModeType type)
{
    Renderer::Submit([type]() { RendererAPI::SetPolygonMode(type); });
}

void Renderer::SetFrontFace(FrontFaceType type)
{
    Renderer::Submit([type]() { RendererAPI::SetFrontFace(type); });
}

void Renderer::SetPrimitiveType(PrimitiveType type)
{
    Renderer::Submit([type]() { RendererAPI::SetPrimitiveType(type); });
}

void Renderer::WaitAndRender()
{
    Get()->m_commandQueue.Execute();
}

} // namespace Doodle
