#pragma once

#include <imgui.h>
#include <rfl/Generic.hpp>

#include <imGuizmo.h>

#include "BaseComponent.h"
#include "CameraComponent.h"
#include "ImGuiUtils.h"
#include "LightComponent.h"
#include "MaterialComponent.h"
#include "MathUtils.h"
#include "MeshComponent.h"
#include "Scene.h"
#include "Scriptable.h"
#include "Transform.h"
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

    void OnInspectorLayout() override;

    rfl::Generic::Object SerializeToObject() const override;

    void DeserializeFromObject(const rfl::Generic::Object &object) override;
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

    void OnInspectorLayout() override;

    rfl::Generic::Object SerializeToObject() const override;

    void DeserializeFromObject(const rfl::Generic::Object &object) override;
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

    void OnInspectorLayout() override;

    rfl::Generic::Object SerializeToObject() const override;

    void DeserializeFromObject(const rfl::Generic::Object &object) override;

    void SetLocalPosition(const glm::vec3 &position);

    void SetLocalRotation(const glm::vec3 &rotation);

    void SetLocalScale(const glm::vec3 &scale);

    void SetLocalScale(float scale);

    void Translate(const glm::vec3 &translation);

    void Rotate(const glm::vec3 &eulerAngles);

    void Scale(const glm::vec3 &scale);

    void Scale(float scale);

    void LookAt(const glm::vec3 &target);

    void UpdateGlobalTransform(const Transform &parentTransform = Transform());

    glm::mat4 GetTransformMatrix() const;

    glm::vec3 GetFront() const;

    glm::vec3 GetRight() const;

    glm::vec3 GetUp() const;

    glm::vec3 GetPosition() const;

    glm::vec3 GetRotation() const;

    glm::quat GetQuaternion() const;

    glm::vec3 GetScale() const;

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

    void Reset();

private:
    glm::mat4 m_parentTransformMatrix = glm::mat4(1.0f);
};

} // namespace Doodle
