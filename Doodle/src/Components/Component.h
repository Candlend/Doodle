#pragma once

#include <glm/detail/type_quat.hpp>
#include <glm/fwd.hpp>
#include <imgui.h>
#include <rfl/Generic.hpp>

#include <imGuizmo.h>

#include "BaseComponent.h"
#include "CameraComponent.h"
#include "ImGuiUtils.h"
#include "LightComponent.h"
#include "MaterialComponent.h"
#include "MathUtils.h"
#include "Renderable.h"
#include "Scriptable.h"
#include "UUID.h"

namespace Doodle
{

struct UUIDComponent : public BaseComponent
{
    COMPONENT_CLASS_TYPE(ID)

    UUID UUID;

    UUIDComponent() = default;
    UUIDComponent(const UUIDComponent &) = default;
    UUIDComponent(const Doodle::UUID &uuid) : UUID(uuid)
    {
    }

    operator Doodle::UUID &()
    {
        return UUID;
    }

    operator const Doodle::UUID &() const
    {
        return UUID;
    }

    void OnInspectorLayout() override
    {
        ImGuiUtils::ReadOnlyInputText("UUID", UUID);
    }

    rfl::Generic::Object SerializeToObject() const override
    {
        rfl::Generic::Object object;
        object["UUID"] = UUID;
        return object;
    }

    void DeserializeFromObject(const rfl::Generic::Object &object) override
    {
        UUID = Doodle::UUID(object.get("UUID").and_then(rfl::to_string).value());
    }
};

struct NameComponent : public BaseComponent
{
    COMPONENT_CLASS_TYPE(Name)

    std::string Name;

    NameComponent() = default;
    NameComponent(const NameComponent &) = default;
    NameComponent(const std::string &name) : Name(name)
    {
    }

    operator std::string &()
    {
        return Name;
    }

    operator const std::string &() const
    {
        return Name;
    }

    void OnInspectorLayout() override
    {
        ImGuiUtils::InputText("Name##NameComponent", Name);
    }

    rfl::Generic::Object SerializeToObject() const override
    {
        rfl::Generic::Object object;
        object["Name"] = Name;
        return object;
    }

    void DeserializeFromObject(const rfl::Generic::Object &object) override
    {
        Name = object.get("Name").and_then(rfl::to_string).value();
    }
};

struct Transform
{
    glm::vec3 Position;
    glm::vec3 Rotation; // Euler angles in degrees
    glm::vec3 Scale;

    Transform() : Position(0.0f), Rotation(0.0f), Scale(1.0f)
    {
    }

    Transform(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale)
        : Position(position), Rotation(rotation), Scale(scale)
    {
    }

    glm::quat GetQuaternion() const
    {
        return glm::quat(glm::radians(Rotation));
    }

    glm::mat4 GetTransformMatrix() const
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
};

struct TransformComponent : public BaseComponent
{
    COMPONENT_CLASS_TYPE(Transform)

    UUID ParentHandle = UUID::Nil();
    std::vector<UUID> Children;
    Transform LocalTransform;
    Transform GlobalTransform;

    bool Dirty = true;

    TransformComponent() = default;

    void OnInspectorLayout() override
    {
        ImGui::Text("Position");
        if (ImGui::DragFloat3("##Position", &LocalTransform.Position.x, 0.1f))
        {
            Dirty = true;
        }
        ImGui::Text("Rotation");
        if (ImGui::DragFloat3("##Rotation", &LocalTransform.Rotation.x, 1.0f))
        {
            Dirty = true;
        }
        ImGui::Text("Scale");
        if (ImGui::DragFloat3("##Scale", &LocalTransform.Scale.x, 0.1f))
        {
            Dirty = true;
        }
        ImGui::Spacing();
        if (ImGui::Button("Reset"))
        {
            Reset();
        }
    }

    rfl::Generic::Object SerializeToObject() const override
    {
        rfl::Generic::Object object;
        rfl::Generic::Object position;
        position["x"] = LocalTransform.Position.x;
        position["y"] = LocalTransform.Position.y;
        position["z"] = LocalTransform.Position.z;
        object["Position"] = position;
        rfl::Generic::Object rotation;
        rotation["x"] = LocalTransform.Rotation.x;
        rotation["y"] = LocalTransform.Rotation.y;
        rotation["z"] = LocalTransform.Rotation.z;
        object["Rotation"] = rotation;
        rfl::Generic::Object scale;
        scale["x"] = LocalTransform.Scale.x;
        scale["y"] = LocalTransform.Scale.y;
        scale["z"] = LocalTransform.Scale.z;
        object["Scale"] = scale;
        return object;
    }

    void DeserializeFromObject(const rfl::Generic::Object &object) override
    {
        auto position = object.get("Position").and_then(rfl::to_object).value();
        LocalTransform.Position.x = position.get("x").and_then(rfl::to_double).value();
        LocalTransform.Position.y = position.get("y").and_then(rfl::to_double).value();
        LocalTransform.Position.z = position.get("z").and_then(rfl::to_double).value();

        auto rotation = object.get("Rotation").and_then(rfl::to_object).value();
        LocalTransform.Rotation.x = rotation.get("x").and_then(rfl::to_double).value();
        LocalTransform.Rotation.y = rotation.get("y").and_then(rfl::to_double).value();
        LocalTransform.Rotation.z = rotation.get("z").and_then(rfl::to_double).value();

        auto scale = object.get("Scale").and_then(rfl::to_object).value();
        LocalTransform.Scale.x = scale.get("x").and_then(rfl::to_double).value();
        LocalTransform.Scale.y = scale.get("y").and_then(rfl::to_double).value();
        LocalTransform.Scale.z = scale.get("z").and_then(rfl::to_double).value();
        Dirty = true;
    }

    void SetLocalPosition(const glm::vec3 &position)
    {
        LocalTransform.Position = position;
        Dirty = true;
    }

    void SetLocalRotation(const glm::vec3 &rotation)
    {
        LocalTransform.Rotation = rotation;
        Dirty = true;
    }

    void SetLocalScale(const glm::vec3 &scale)
    {
        LocalTransform.Scale = scale;
        Dirty = true;
    }

    void SetLocalScale(float scale)
    {
        LocalTransform.Scale = glm::vec3(scale);
        Dirty = true;
    }

    void Translate(const glm::vec3 &translation)
    {
        LocalTransform.Translate(translation);
        Dirty = true;
    }

    void Rotate(const glm::vec3 &eulerAngles)
    {
        LocalTransform.Rotate(eulerAngles);
        Dirty = true;
    }

    void Scale(const glm::vec3 &scale)
    {
        LocalTransform.ScaleBy(scale);
        Dirty = true;
    }

    void Scale(float scale)
    {
        LocalTransform.ScaleBy(scale);
        Dirty = true;
    }

    void LookAt(const glm::vec3 &target)
    {
        if (Dirty)
        {
            GetScene()->UpdateGlobalTransforms();
        }
        auto localTarget =
            glm::inverse(m_parentTransformMatrix) * glm::vec4(target, 1.0f); // TODO - Check if this is correct
        LocalTransform.LookAt(localTarget);
        Dirty = true;
    }

    void UpdateGlobalTransform(const Transform &parentTransform = Transform())
    {
        m_parentTransformMatrix = parentTransform.GetTransformMatrix();
        GlobalTransform.Position = m_parentTransformMatrix * glm::vec4(LocalTransform.Position, 1.0f);
        GlobalTransform.Rotation = parentTransform.Rotation + LocalTransform.Rotation;
        GlobalTransform.Scale = parentTransform.Scale * LocalTransform.Scale;
        Dirty = false;
    }

    glm::mat4 GetTransformMatrix() const
    {
        if (Dirty)
        {
            GetScene()->UpdateGlobalTransforms();
        }
        return GlobalTransform.GetTransformMatrix();
    }

    glm::vec3 GetFront() const
    {
        if (Dirty)
        {
            GetScene()->UpdateGlobalTransforms();
        }
        return GlobalTransform.GetFront();
    }

    glm::vec3 GetRight() const
    {
        if (Dirty)
        {
            GetScene()->UpdateGlobalTransforms();
        }
        return GlobalTransform.GetRight();
    }

    glm::vec3 GetUp() const
    {
        if (Dirty)
        {
            GetScene()->UpdateGlobalTransforms();
        }
        return GlobalTransform.GetUp();
    }

    glm::vec3 GetPosition() const
    {
        if (Dirty)
        {
            GetScene()->UpdateGlobalTransforms();
        }
        return GlobalTransform.Position;
    }

    glm::vec3 GetRotation() const
    {
        if (Dirty)
        {
            GetScene()->UpdateGlobalTransforms();
        }
        return GlobalTransform.Rotation;
    }

    glm::quat GetQuaternion() const
    {
        if (Dirty)
        {
            GetScene()->UpdateGlobalTransforms();
        }
        return GlobalTransform.GetQuaternion();
    }

    glm::vec3 GetScale() const
    {
        if (Dirty)
        {
            GetScene()->UpdateGlobalTransforms();
        }
        return GlobalTransform.Scale;
    }

    glm::vec3 GetLocalPosition() const
    {
        return LocalTransform.Position;
    }

    glm::vec3 GetLocalRotation() const
    {
        return LocalTransform.Rotation;
    }

    glm::vec3 GetLocalScale() const
    {
        return LocalTransform.Scale;
    }

    glm::vec3 GetLocalFront() const
    {
        return LocalTransform.GetFront();
    }

    glm::vec3 GetLocalRight() const
    {
        return LocalTransform.GetRight();
    }

    glm::vec3 GetLocalUp() const
    {
        return LocalTransform.GetUp();
    }

    void Reset()
    {
        LocalTransform.Reset();
        Dirty = true;
    }

private:
    glm::mat4 m_parentTransformMatrix = glm::mat4(1.0f);
};

} // namespace Doodle
