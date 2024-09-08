
#include "SceneCamera.h"
#include "PanelManager.h"
#include "ViewportPanel.h"

namespace Doodle
{
std::shared_ptr<SceneCamera> Create(ProjectionType type)
{
    return std::make_shared<SceneCamera>(type);
}

SceneCamera::SceneCamera(ProjectionType type) : m_projectionType(type)
{
    switch (m_projectionType)
    {
    case ProjectionType::Perspective:
        SetPerspective(45.0f, 0.1f, 1000.0f);
        break;
    case ProjectionType::Orthographic:
        SetOrthographic(10.0f, -1.0f, 1.0f);
        break;
    }
    SetViewportSize(1920, 1080);
}

void SceneCamera::SetPerspective(float degVerticalFov, float nearClip, float farClip)
{
    m_projectionType = ProjectionType::Perspective;
    m_degPerspectiveFov = degVerticalFov;
    m_perspectiveNear = nearClip;
    m_perspectiveFar = farClip;
}

void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
{
    m_projectionType = ProjectionType::Orthographic;
    m_orthographicSize = size;
    m_orthographicNear = nearClip;
    m_orthographicFar = farClip;
}

void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
{
    switch (m_projectionType)
    {
    case ProjectionType::Perspective:
        SetPerspectiveProjectionMatrix(glm::radians(m_degPerspectiveFov), static_cast<float>(width),
                                       static_cast<float>(height), m_perspectiveNear, m_perspectiveFar);
        break;
    case ProjectionType::Orthographic:
        float aspect = static_cast<float>(width) / static_cast<float>(height);
        float width = m_orthographicSize * aspect;
        float height = m_orthographicSize;
        SetOrthoProjectionMatrix(width, height, m_orthographicNear, m_orthographicFar);
        break;
    }
}

} // namespace Doodle