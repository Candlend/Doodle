
#include "Camera.h"

namespace Doodle
{

Camera::Camera(const glm::mat4 &projection) : m_projectionMatrix(projection)
{
}

Camera::Camera(const float degFov, const float width, const float height, const float nearP, const float farP)
    : m_projectionMatrix(glm::perspectiveFov(glm::radians(degFov), width, height, farP, nearP))
{
}

} // namespace Doodle