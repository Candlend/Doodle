#include "Renderer.h"
#include "pch.h"
#include <functional>

namespace Doodle
{

void Renderer::Initialize()
{
    Renderer::Get().Submit(std::function(RendererAPI::Initialize));
}

void Renderer::Deinitialize()
{
}

void Renderer::Clear(float r, float g, float b, float a)
{
    Renderer::Get().Submit(std::function(RendererAPI::Clear), r, g, b, a);
}

void Renderer::DrawIndexed(unsigned int count)
{
    Renderer::Get().Submit(std::function(RendererAPI::DrawIndexed), count);
}

void Renderer::WaitAndRender()
{
    Get().m_commandQueue.Execute();
}

} // namespace Doodle
