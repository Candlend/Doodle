#pragma once

#include "BaseComponent.h"
#include "CameraComponent.h"
#include "ImGuiUtils.h"
#include "LightComponent.h"
#include "MaterialComponent.h"
#include "Renderable.h"
#include "Scriptable.h"

namespace Doodle
{

struct IDComponent : public BaseComponent
{
    COMPONENT_CLASS_TYPE(ID)

    UUID ID;

    IDComponent() = default;
    IDComponent(const IDComponent &) = default;
    IDComponent(const UUID &id) : ID(id)
    {
    }

    operator UUID &()
    {
        return ID;
    }

    operator const UUID &() const
    {
        return ID;
    }

    void OnInspectorLayout() override
    {
        ImGuiUtils::ReadOnlyInputText("ID", ID);
    }
};

struct TagComponent : public BaseComponent
{
    COMPONENT_CLASS_TYPE(Tag)

    std::string Tag;

    TagComponent() = default;
    TagComponent(const TagComponent &) = default;
    TagComponent(const std::string &tag) : Tag(tag)
    {
    }

    operator std::string &()
    {
        return Tag;
    }

    operator const std::string &() const
    {
        return Tag;
    }

    void OnInspectorLayout() override
    {
        ImGuiUtils::InputText("Tag##TagComponent", Tag);
    }
};

struct TransformComponent : public BaseComponent
{
    COMPONENT_CLASS_TYPE(Transform)

    glm::vec3 Position;
    glm::vec3 Rotation; // Euler angles in degrees
    glm::vec3 Scale;

    TransformComponent() : Position(0.0f), Rotation(0.0f), Scale(1.0f)
    {
    }

    glm::quat GetRotation() const
    {
        return glm::quat(glm::radians(Rotation));
    }

    glm::mat4 GetModelMatrix() const
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, Position);
        model *= glm::toMat4(glm::quat(glm::radians(Rotation))); // Use quaternion for rotation
        model = glm::scale(model, Scale);
        return model;
    }

    glm::vec3 GetFront() const
    {
        glm::vec3 front(0.0f, 0.0f, -1.0f); // Assuming -Z is front
        glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(Rotation)));
        return glm::normalize(glm::vec3(rotation * glm::vec4(front, 1.0f)));
    }

    glm::vec3 GetRight() const
    {
        glm::vec3 front = GetFront();
        glm::vec3 up(0.0f, 1.0f, 0.0f); // Assuming Y is up
        return glm::normalize(glm::cross(front, up));
    }

    glm::vec3 GetUp() const
    {
        glm::vec3 front = GetFront();
        glm::vec3 right = GetRight();
        return glm::normalize(glm::cross(right, front));
    }

    void LookAt(const glm::vec3 &target)
    {
        glm::vec3 direction = glm::normalize(target - Position);

        float pitch = glm::asin(direction.y);
        float yaw = glm::atan(-direction.x, -direction.z);

        Rotation.x = glm::degrees(pitch);
        Rotation.y = glm::degrees(yaw);
        Rotation.z = 0.0f;
    }

    void Rotate(const glm::vec3 &eulerAngles)
    {
        glm::quat currentRotation = glm::quat(glm::radians(Rotation));
        glm::quat additionalRotation = glm::quat(glm::radians(eulerAngles));
        Rotation = glm::degrees(glm::eulerAngles(currentRotation * additionalRotation));
    }

    void Translate(const glm::vec3 &translation)
    {
        Position += translation;
    }

    void ScaleBy(const glm::vec3 &scale)
    {
        Scale *= scale;
    }

    void ScaleBy(float scale)
    {
        Scale *= scale;
    }

    void Reset()
    {
        Position = glm::vec3(0.0f);
        Rotation = glm::vec3(0.0f);
        Scale = glm::vec3(1.0f);
    }

    void ResetPosition()
    {
        Position = glm::vec3(0.0f);
    }

    void ResetRotation()
    {
        Rotation = glm::vec3(0.0f);
    }

    void ResetScale()
    {
        Scale = glm::vec3(1.0f);
    }

    void OnInspectorLayout() override
    {
        ImGui::Text("Position");
        ImGui::DragFloat3("##Position", &Position.x, 0.1f);
        ImGui::Text("Rotation");
        ImGui::DragFloat3("##Rotation", &Rotation.x, 1.0f);
        ImGui::Text("Scale");
        ImGui::DragFloat3("##Scale", &Scale.x, 0.1f);
    }
};

} // namespace Doodle