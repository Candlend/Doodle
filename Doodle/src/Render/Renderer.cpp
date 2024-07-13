#include "Renderer.h"
#include "Log.h"
#include "pch.h"
#include <functional>

namespace Doodle
{

void Renderer::Initialize()
{
    Renderer::Submit([]() {
        RendererAPI::Initialize();
        DOO_CORE_TRACE("Renderer initialized");
    });
}

void Renderer::Deinitialize()
{
}

void Renderer::Clear(float r, float g, float b, float a)
{
    Renderer::Submit([r, g, b, a]() {
        RendererAPI::Clear(r, g, b, a);
        DOO_CORE_TRACE("Renderer cleared: {0}, {1}, {2}, {3}", r, g, b, a);
    });
}

void Renderer::DrawIndexed(unsigned int count, bool depthTest)
{
    Renderer::Submit([count, depthTest]() {
        RendererAPI::DrawIndexed(count, depthTest);
        DOO_CORE_TRACE("Renderer draw indexed: {0}, {1}", count, depthTest);
    });
}

void Renderer::WaitAndRender()
{
    Get().m_commandQueue.Execute();
}

// void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
// {
//     shader->Bind();
//     std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection",
//     s_SceneData->ViewProjectionMatrix);
//     std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);

//     vertexArray->Bind();
//     RenderCommand::DrawIndexed(vertexArray);
// }

} // namespace Doodle
