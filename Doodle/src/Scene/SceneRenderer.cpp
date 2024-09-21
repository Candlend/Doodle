#include "pch.h"
#include <glad/glad.h>
#include <memory>

#include "ApplicationEvent.h"
#include "Component.h"
#include "EditorCamera.h"
#include "Entity.h"
#include "EventManager.h"
#include "FrameBuffer.h"
#include "PanelManager.h"
#include "RenderPipeline.h"
#include "RenderScope.h"
#include "SceneManager.h"
#include "SceneRenderer.h"
#include "ViewportPanel.h"

namespace Doodle
{

SceneRenderer::SceneRenderer()
{
    FramebufferAttachmentSpecification attachments = {FramebufferTextureFormat::RGBA16F,
                                                      FramebufferTextureFormat::RED_INTEGER,
                                                      FramebufferTextureFormat::DEPTH24STENCIL8};
    FramebufferSpecification spec = {1920, 1080, attachments};
    m_frameBuffer = FrameBuffer::Create(spec);

    EventManager::Get()->AddListener<AppRenderEvent>(this, &SceneRenderer::Render);

    RenderPipeline::Get()->SetTargetFrameBuffer(m_frameBuffer);
    RenderPipeline::Get()->RegisterRenderPasses();
}

SceneRenderer::~SceneRenderer()
{
    EventManager::Get()->RemoveListener<AppRenderEvent>(this, &SceneRenderer::Render);
}

void SceneRenderer::Render()
{
    RenderScope<FrameBuffer> renderScope(m_frameBuffer.get());
    Renderer::SetClearColor(0.3, 0.3, 0.3, 1.0);
    Renderer::Clear();
    auto scene = SceneManager::Get()->GetActiveScene();
    if (!scene || !scene->IsActive())
    {
        return;
    }
    RenderPipeline::Get()->Execute();
}

bool SceneRenderer::OnEvent(ViewportResizeEvent &e)
{
    m_frameBuffer->Resize(e.GetWidth(), e.GetHeight());
    return false;
}

bool SceneRenderer::OnEvent(SceneActivateEvent &e)
{
    auto *scene = e.GetScene();
    RenderPipeline::Get()->BeginScene(scene);
    return false;
}

bool SceneRenderer::OnEvent(SceneDeactivateEvent &e)
{
    auto *scene = e.GetScene();
    RenderPipeline::Get()->EndScene();
    return false;
}

} // namespace Doodle
