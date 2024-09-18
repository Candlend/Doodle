#pragma once

#include "pch.h"

#include "Camera.h"

namespace Doodle
{

enum class ProjectionType
{
    Perspective = 0,
    Orthographic = 1
};

class DOO_API SceneCamera : public Camera
{
public:
    static std::shared_ptr<SceneCamera> Create(ProjectionType type = ProjectionType::Perspective);

    SceneCamera() = default;
    SceneCamera(ProjectionType type);

    void SetPerspective(float verticalFov, float nearClip = 0.1f, float farClip = 1000.0f);
    void SetOrthographic(float size, float nearClip = -1.0f, float farClip = 1.0f);
    void SetViewportSize(uint32_t width, uint32_t height);

    void SetDegPerspectiveVerticalFov(const float degVerticalFov)
    {
        m_degPerspectiveFov = degVerticalFov;
    }
    void SetRadPerspectiveVerticalFov(const float degVerticalFov)
    {
        m_degPerspectiveFov = glm::degrees(degVerticalFov);
    }
    float GetDegPerspectiveVerticalFov() const
    {
        return m_degPerspectiveFov;
    }
    float GetRadPerspectiveVerticalFov() const
    {
        return glm::radians(m_degPerspectiveFov);
    }
    void SetPerspectiveNearClip(const float nearClip)
    {
        m_perspectiveNear = nearClip;
    }
    float GetPerspectiveNearClip() const
    {
        return m_perspectiveNear;
    }
    void SetPerspectiveFarClip(const float farClip)
    {
        m_perspectiveFar = farClip;
    }
    float GetPerspectiveFarClip() const
    {
        return m_perspectiveFar;
    }

    void SetOrthographicSize(const float size)
    {
        m_orthographicSize = size;
    }
    float GetOrthographicSize() const
    {
        return m_orthographicSize;
    }
    void SetOrthographicNearClip(const float nearClip)
    {
        m_orthographicNear = nearClip;
    }
    float GetOrthographicNearClip() const
    {
        return m_orthographicNear;
    }
    void SetOrthographicFarClip(const float farClip)
    {
        m_orthographicFar = farClip;
    }
    float GetOrthographicFarClip() const
    {
        return m_orthographicFar;
    }

    void SetProjectionType(ProjectionType type)
    {
        m_projectionType = type;
    }
    ProjectionType GetProjectionType() const
    {
        return m_projectionType;
    }
    float GetNearClip() const
    {
        return m_projectionType == ProjectionType::Perspective ? m_perspectiveNear : m_orthographicNear;
    }
    float GetFarClip() const
    {
        return m_projectionType == ProjectionType::Perspective ? m_perspectiveFar : m_orthographicFar;
    }

private:
    ProjectionType m_projectionType = ProjectionType::Perspective;

    float m_degPerspectiveFov = 45.0f;
    float m_perspectiveNear = 0.1f, m_perspectiveFar = 1000.0f;

    float m_orthographicSize = 10.0f;
    float m_orthographicNear = -1.0f, m_orthographicFar = 1.0f;
};

} // namespace Doodle