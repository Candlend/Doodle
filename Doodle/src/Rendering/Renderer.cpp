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

void Renderer::DrawIndexed(unsigned int count, bool depthTest)
{
    Renderer::Submit([count, depthTest]() {
        RendererAPI::DrawIndexed(count, depthTest);
        DOO_CORE_TRACE("Renderer draw indexed: {0}, {1}", count, depthTest);
    });
}

void Renderer::UseWireframe(bool useWireframe)
{
    Renderer::Submit([useWireframe]() {
        RendererAPI::UseWireframe(useWireframe);
        DOO_CORE_TRACE("Renderer use wireframe: {0}", useWireframe);
    });
}

void Renderer::WaitAndRender()
{
    Get()->m_commandQueue.Execute();
}

} // namespace Doodle
