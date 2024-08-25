#pragma once

#include "pch.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Doodle
{

class DOO_API Camera
{
public:
    Camera() = default;
    explicit Camera(const glm::mat4 &projection);
    Camera(float degFov, float width, float height, float nearP, float farP);
    virtual ~Camera() = default;

    const glm::mat4 &GetProjectionMatrix() const
    {
        return m_projectionMatrix;
    }

    void SetProjectionMatrix(const glm::mat4 projection)
    {
        m_projectionMatrix = projection;
    }

    void SetPerspectiveProjectionMatrix(const float radFov, const float width, const float height, const float nearP,
                                        const float farP)
    {
        m_projectionMatrix = glm::perspectiveFov(radFov, width, height, farP, nearP);
    }

    void SetOrthoProjectionMatrix(const float width, const float height, const float nearP, const float farP)
    {
        m_projectionMatrix = glm::ortho(-width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, farP, nearP);
    }

    float GetExposure() const
    {
        return m_exposure;
    }
    float &GetExposure()
    {
        return m_exposure;
    }

protected:
    float m_exposure = 0.8f;

private:
    glm::mat4 m_projectionMatrix = glm::mat4(1.0f);
};

} // namespace Doodle