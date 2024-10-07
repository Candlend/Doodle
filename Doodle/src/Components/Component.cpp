#include "Component.h"

namespace Doodle
{

void UUIDComponent::OnInspectorLayout()
{
    ImGuiUtils::ReadOnlyInputText("UUID", UUID);
}

rfl::Generic::Object UUIDComponent::SerializeToObject() const
{
    rfl::Generic::Object object;
    object["UUID"] = UUID;
    return object;
}

void UUIDComponent::DeserializeFromObject(const rfl::Generic::Object &object)
{
    UUID = Doodle::UUID(object.get("UUID").and_then(rfl::to_string).value());
}

void NameComponent::OnInspectorLayout()
{
    ImGuiUtils::InputText("Name##NameComponent", Name);
}

rfl::Generic::Object NameComponent::SerializeToObject() const
{
    rfl::Generic::Object object;
    object["Name"] = Name;
    return object;
}

void NameComponent::DeserializeFromObject(const rfl::Generic::Object &object)
{
    Name = object.get("Name").and_then(rfl::to_string).value();
}

void TransformComponent::OnInspectorLayout()
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

rfl::Generic::Object TransformComponent::SerializeToObject() const
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

void TransformComponent::DeserializeFromObject(const rfl::Generic::Object &object)
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

void TransformComponent::SetLocalPosition(const glm::vec3 &position)
{
    LocalTransform.Position = position;
    Dirty = true;
}

void TransformComponent::SetLocalRotation(const glm::vec3 &rotation)
{
    LocalTransform.Rotation = rotation;
    Dirty = true;
}

void TransformComponent::SetLocalScale(const glm::vec3 &scale)
{
    LocalTransform.Scale = scale;
    Dirty = true;
}

void TransformComponent::SetLocalScale(float scale)
{
    LocalTransform.Scale = glm::vec3(scale);
    Dirty = true;
}

void TransformComponent::Translate(const glm::vec3 &translation)
{
    LocalTransform.Translate(translation);
    Dirty = true;
}

void TransformComponent::Rotate(const glm::vec3 &eulerAngles)
{
    LocalTransform.Rotate(eulerAngles);
    Dirty = true;
}

void TransformComponent::Scale(const glm::vec3 &scale)
{
    LocalTransform.ScaleBy(scale);
    Dirty = true;
}

void TransformComponent::Scale(float scale)
{
    LocalTransform.ScaleBy(scale);
    Dirty = true;
}

void TransformComponent::LookAt(const glm::vec3 &target)
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

void TransformComponent::UpdateGlobalTransform(const Transform &parentTransform)
{
    m_parentTransformMatrix = parentTransform.GetTransformMatrix();
    GlobalTransform.Position = m_parentTransformMatrix * glm::vec4(LocalTransform.Position, 1.0f);
    GlobalTransform.Rotation = parentTransform.Rotation + LocalTransform.Rotation;
    GlobalTransform.Scale = parentTransform.Scale * LocalTransform.Scale;
    Dirty = false;
}

glm::mat4 TransformComponent::GetTransformMatrix() const
{
    if (Dirty)
    {
        GetScene()->UpdateGlobalTransforms();
    }
    return GlobalTransform.GetTransformMatrix();
}

glm::vec3 TransformComponent::GetFront() const
{
    if (Dirty)
    {
        GetScene()->UpdateGlobalTransforms();
    }
    return GlobalTransform.GetFront();
}

glm::vec3 TransformComponent::GetRight() const
{
    if (Dirty)
    {
        GetScene()->UpdateGlobalTransforms();
    }
    return GlobalTransform.GetRight();
}

glm::vec3 TransformComponent::GetUp() const
{
    if (Dirty)
    {
        GetScene()->UpdateGlobalTransforms();
    }
    return GlobalTransform.GetUp();
}

glm::vec3 TransformComponent::GetPosition() const
{
    if (Dirty)
    {
        GetScene()->UpdateGlobalTransforms();
    }
    return GlobalTransform.Position;
}

glm::vec3 TransformComponent::GetRotation() const
{
    if (Dirty)
    {
        GetScene()->UpdateGlobalTransforms();
    }
    return GlobalTransform.Rotation;
}

glm::quat TransformComponent::GetQuaternion() const
{
    if (Dirty)
    {
        GetScene()->UpdateGlobalTransforms();
    }
    return GlobalTransform.GetQuaternion();
}

glm::vec3 TransformComponent::GetScale() const
{
    if (Dirty)
    {
        GetScene()->UpdateGlobalTransforms();
    }
    return GlobalTransform.Scale;
}

void TransformComponent::Reset()
{
    LocalTransform.Reset();
    Dirty = true;
}

} // namespace Doodle
