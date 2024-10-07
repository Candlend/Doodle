#include "Transform.h"

namespace Doodle
{

glm::quat Transform::GetQuaternion() const
{
    return glm::quat(glm::radians(Rotation));
}

glm::mat4 Transform::GetTransformMatrix() const
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, Position);
    model *= glm::toMat4(glm::quat(glm::radians(Rotation))); // Use quaternion for rotation
    model = glm::scale(model, Scale);
    return model;
}

glm::vec3 Transform::GetFront() const
{
    glm::vec3 front(0.0f, 0.0f, -1.0f); // Assuming -Z is front
    glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(Rotation)));
    return glm::normalize(glm::vec3(rotation * glm::vec4(front, 1.0f)));
}

glm::vec3 Transform::GetRight() const
{
    glm::vec3 front = GetFront();
    glm::vec3 up(0.0f, 1.0f, 0.0f); // Assuming Y is up
    return glm::normalize(glm::cross(front, up));
}

glm::vec3 Transform::GetUp() const
{
    glm::vec3 front = GetFront();
    glm::vec3 right = GetRight();
    return glm::normalize(glm::cross(right, front));
}

void Transform::LookAt(const glm::vec3 &target)
{
    glm::vec3 direction = glm::normalize(target - Position);

    float pitch = glm::asin(direction.y);
    float yaw = glm::atan(-direction.x, -direction.z);

    Rotation.x = glm::degrees(pitch);
    Rotation.y = glm::degrees(yaw);
    Rotation.z = 0.0f;
}

void Transform::Rotate(const glm::vec3 &eulerAngles)
{
    glm::quat currentRotation = glm::quat(glm::radians(Rotation));
    glm::quat additionalRotation = glm::quat(glm::radians(eulerAngles));
    Rotation = glm::degrees(glm::eulerAngles(currentRotation * additionalRotation));
}

void Transform::Reset()
{
    Position = glm::vec3(0.0f);
    Rotation = glm::vec3(0.0f);
    Scale = glm::vec3(1.0f);
}

} // namespace Doodle
