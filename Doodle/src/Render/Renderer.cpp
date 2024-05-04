#include "Renderer.h"
#include "pch.h"

namespace Doodle
{

void Renderer::Initialize()
{
    // Initialization logic
}

void Renderer::Clear()
{
    // Clear logic
}

void Renderer::Clear(float r, float g, float b, float a)
{
    RendererAPI::Clear(r, g, b, a);
}

void Renderer::ClearMagenta()
{
    Clear(1, 0, 1);
}

void Renderer::SetClearColor(float r, float g, float b, float a)
{
    // Set clear color logic
}

void Renderer::WaitAndRender()
{
    Get().m_commandQueue.Execute();
}

} // namespace Doodle
