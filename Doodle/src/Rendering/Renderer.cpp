#include "pch.h"
#include <glad/glad.h>

#include "ApplicationEvent.h"
#include "EventManager.h"
#include "Log.h"
#include "Renderer.h"

namespace Doodle
{

void Renderer::Initialize()
{
    Renderer::Submit([]() {
        RendererAPI::Initialize();
        DOO_CORE_TRACE("Renderer initialized");
    });

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

void Renderer::Clear(float r, float g, float b, float a)
{
    Renderer::Submit([r, g, b, a]() {
        RendererAPI::Clear(r, g, b, a);
        DOO_CORE_TRACE("Renderer cleared: {0}, {1}, {2}, {3}", r, g, b, a);
    });
}

void Renderer::Clear()
{
    Renderer::Submit([]() {
        RendererAPI::Clear();
        DOO_CORE_TRACE("Renderer cleared");
    });
}

void Renderer::SetClearColor(float r, float g, float b, float a)
{
    Renderer::Submit([r, g, b, a]() {
        RendererAPI::SetClearColor(r, g, b, a);
        DOO_CORE_TRACE("Renderer set clear color: {0}, {1}, {2}, {3}", r, g, b, a);
    });
}

void Renderer::DrawIndexed(unsigned int count)
{
    Renderer::Submit([count]() {
        RendererAPI::DrawIndexed(count);
        DOO_CORE_TRACE("Renderer draw indexed: {0}", count);
    });
}

void Renderer::Draw(unsigned int count, PrimitiveType type)
{
    Renderer::Submit([count, type]() {
        RendererAPI::Draw(count, type);
        DOO_CORE_TRACE("Renderer draw triangles: {0}", count);
    });
}

void Renderer::SetDepthTest(DepthTestType type)
{
    Renderer::Submit([type]() {
        RendererAPI::SetDepthTest(type);
        DOO_CORE_TRACE("Renderer set depth test: {0}", static_cast<int>(type));
    });
}

void Renderer::SetDepthWrite(bool write)
{
    Renderer::Submit([write]() {
        RendererAPI::SetDepthWrite(write);
        DOO_CORE_TRACE("Renderer set depth write: {0}", write);
    });
}

void Renderer::SetCullFace(CullFaceType type)
{
    Renderer::Submit([type]() {
        RendererAPI::SetCullFace(type);
        DOO_CORE_TRACE("Renderer set cull face: {0}", static_cast<int>(type));
    });
}

void Renderer::SetBlend(BlendType src, BlendType dst)
{
    Renderer::Submit([src, dst]() {
        RendererAPI::SetBlend(src, dst);
        DOO_CORE_TRACE("Renderer set blend: {0}, {1}", static_cast<int>(src), static_cast<int>(dst));
    });
}

void Renderer::SetBlendEquation(BlendEquationType type)
{
    Renderer::Submit([type]() {
        RendererAPI::SetBlendEquation(type);
        DOO_CORE_TRACE("Renderer set blend equation: {0}", static_cast<int>(type));
    });
}

void Renderer::SetPolygonMode(PolygonModeType type)
{
    Renderer::Submit([type]() {
        RendererAPI::SetPolygonMode(type);
        DOO_CORE_TRACE("Renderer set polygon mode: {0}", static_cast<int>(type));
    });
}

void Renderer::SetFrontFace(FrontFaceType type)
{
    Renderer::Submit([type]() {
        RendererAPI::SetFrontFace(type);
        DOO_CORE_TRACE("Renderer set front face: {0}", static_cast<int>(type));
    });
}

void Renderer::SetPrimitiveType(PrimitiveType type)
{
    Renderer::Submit([type]() {
        RendererAPI::SetPrimitiveType(type);
        DOO_CORE_TRACE("Renderer set primitive type: {0}", static_cast<int>(type));
    });
}

void Renderer::WaitAndRender()
{
    Get()->m_commandQueue.Execute();
}

} // namespace Doodle
