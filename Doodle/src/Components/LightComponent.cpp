#include "LightComponent.h"

namespace Doodle
{

void DirectionalLightComponent::OnInspectorLayout()
{
    ImGui::ColorEdit3("Radiance", glm::value_ptr(Radiance));
    ImGui::DragFloat("Intensity", &Intensity, 0.1f, 0.0f, 100.0f);
}

void DirectionalLightComponent::OnDrawGizmos(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection)
{
    ImU32 color = ImGuiUtils::GetHexColor(ImColor(Radiance.x, Radiance.y, Radiance.z));
    glm::mat4 modelNoScale = RemoveScaling(model);
    ImGuizmo::DrawDirectionalLightGizmos(glm::value_ptr(view), glm::value_ptr(projection), glm::value_ptr(modelNoScale),
                                         1, color, 0.3f, 1.0f);
}

rfl::Generic::Object DirectionalLightComponent::SerializeToObject() const
{
    rfl::Generic::Object object;
    auto radiance = rfl::Generic::Object();
    radiance["x"] = Radiance.x;
    radiance["y"] = Radiance.y;
    radiance["z"] = Radiance.z;
    object["Radiance"] = radiance;
    object["Intensity"] = Intensity;
    return object;
}

void DirectionalLightComponent::DeserializeFromObject(const rfl::Generic::Object &object)
{
    auto radiance = object.get("Radiance").and_then(rfl::to_object).value();
    Radiance.x = radiance.get("x").and_then(rfl::to_double).value();
    Radiance.y = radiance.get("y").and_then(rfl::to_double).value();
    Radiance.z = radiance.get("z").and_then(rfl::to_double).value();
    Intensity = object.get("Intensity").and_then(rfl::to_double).value();
}

void PointLightComponent::OnInspectorLayout()
{
    ImGui::ColorEdit3("Radiance", glm::value_ptr(Radiance));
    ImGui::DragFloat("Intensity", &Intensity, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloatRange2("Range", &MinRange, &Range, 0.1f, 0.0f, 100.0f);
}

void PointLightComponent::OnDrawGizmos(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection)
{
    ImU32 color = ImGuiUtils::GetHexColor(ImColor(Radiance.x, Radiance.y, Radiance.z));
    glm::mat4 modelNoScale = RemoveScaling(model);
    ImGuizmo::DrawPointLightGizmos(glm::value_ptr(view), glm::value_ptr(projection), glm::value_ptr(modelNoScale), 1,
                                   color, Range);
}

rfl::Generic::Object PointLightComponent::SerializeToObject() const
{
    rfl::Generic::Object object;
    auto radiance = rfl::Generic::Object();
    radiance["x"] = Radiance.x;
    radiance["y"] = Radiance.y;
    radiance["z"] = Radiance.z;
    object["Radiance"] = radiance;
    object["Intensity"] = Intensity;
    object["MinRange"] = MinRange;
    object["Range"] = Range;
    return object;
}

void PointLightComponent::DeserializeFromObject(const rfl::Generic::Object &object)
{
    auto radiance = object.get("Radiance").and_then(rfl::to_object).value();
    Radiance.x = radiance.get("x").and_then(rfl::to_double).value();
    Radiance.y = radiance.get("y").and_then(rfl::to_double).value();
    Radiance.z = radiance.get("z").and_then(rfl::to_double).value();
    Intensity = object.get("Intensity").and_then(rfl::to_double).value();
    MinRange = object.get("MinRange").and_then(rfl::to_double).value();
    Range = object.get("Range").and_then(rfl::to_double).value();
}

void SpotLightComponent::OnInspectorLayout()
{
    ImGui::ColorEdit3("Radiance", glm::value_ptr(Radiance));
    ImGui::DragFloat("Intensity", &Intensity, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloatRange2("Range", &MinRange, &Range, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloatRange2("Angle", &MinAngle, &Angle, 0.1f, 0.0f, 180.0f);
}

void SpotLightComponent::OnDrawGizmos(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection)
{
    ImU32 color = ImGuiUtils::GetHexColor(ImColor(Radiance.x, Radiance.y, Radiance.z));
    glm::mat4 modelNoScale = RemoveScaling(model);
    ImGuizmo::DrawSpotLightGizmos(glm::value_ptr(view), glm::value_ptr(projection), glm::value_ptr(model), 1, color,
                                  Range, Angle);
}

rfl::Generic::Object SpotLightComponent::SerializeToObject() const
{
    rfl::Generic::Object object;
    auto radiance = rfl::Generic::Object();
    radiance["x"] = Radiance.x;
    radiance["y"] = Radiance.y;
    radiance["z"] = Radiance.z;
    object["Radiance"] = radiance;
    object["Intensity"] = Intensity;
    object["MinRange"] = MinRange;
    object["Range"] = Range;
    object["MinAngle"] = MinAngle;
    object["Angle"] = Angle;
    return object;
}

void SpotLightComponent::DeserializeFromObject(const rfl::Generic::Object &object)
{
    auto radiance = object.get("Radiance").and_then(rfl::to_object).value();
    Radiance.x = radiance.get("x").and_then(rfl::to_double).value();
    Radiance.y = radiance.get("y").and_then(rfl::to_double).value();
    Radiance.z = radiance.get("z").and_then(rfl::to_double).value();
    Intensity = object.get("Intensity").and_then(rfl::to_double).value();
    MinRange = object.get("MinRange").and_then(rfl::to_double).value();
    Range = object.get("Range").and_then(rfl::to_double).value();
    MinAngle = object.get("MinAngle").and_then(rfl::to_double).value();
    Angle = object.get("Angle").and_then(rfl::to_double).value();
}

void AreaLightComponent::OnInspectorLayout()
{
    ImGui::ColorEdit3("Radiance", glm::value_ptr(Radiance));
    ImGui::DragFloat("Intensity", &Intensity, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat2("Size", glm::value_ptr(Size), 0.1f, 0.0f, 100.0f);
    ImGui::Checkbox("Two Sided", &TwoSided);
}

void AreaLightComponent::OnDrawGizmos(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection)
{
    ImU32 color = ImGuiUtils::GetHexColor(ImColor(Radiance.x, Radiance.y, Radiance.z));
    ImGuizmo::DrawAreaLightGizmos(glm::value_ptr(view), glm::value_ptr(projection), glm::value_ptr(model), 1, color,
                                  Size.x, Size.y);
}

rfl::Generic::Object AreaLightComponent::SerializeToObject() const
{
    rfl::Generic::Object object;
    auto radiance = rfl::Generic::Object();
    radiance["x"] = Radiance.x;
    radiance["y"] = Radiance.y;
    radiance["z"] = Radiance.z;
    object["Radiance"] = radiance;
    object["Intensity"] = Intensity;
    auto size = rfl::Generic::Object();
    size["x"] = Size.x;
    size["y"] = Size.y;
    object["Size"] = size;
    object["TwoSided"] = TwoSided;
    return object;
}

void AreaLightComponent::DeserializeFromObject(const rfl::Generic::Object &object)
{
    auto radiance = object.get("Radiance").and_then(rfl::to_object).value();
    Radiance.x = radiance.get("x").and_then(rfl::to_double).value();
    Radiance.y = radiance.get("y").and_then(rfl::to_double).value();
    Radiance.z = radiance.get("z").and_then(rfl::to_double).value();
    Intensity = object.get("Intensity").and_then(rfl::to_double).value();
    auto size = object.get("Size").and_then(rfl::to_object).value();
    Size.x = size.get("x").and_then(rfl::to_double).value();
    Size.y = size.get("y").and_then(rfl::to_double).value();
    TwoSided = object.get("TwoSided").and_then(rfl::to_bool).value();
}
} // namespace Doodle
