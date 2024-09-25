#pragma once

#include "pch.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include <imGuizmo.h>

#include "BaseComponent.h"
#include "ImGuiUtils.h"
#include "MathUtils.h"

namespace Doodle
{

struct DirectionalLightComponent : public BaseComponent
{
    COMPONENT_CLASS_TYPE(DirectionalLight)

    glm::vec3 Radiance{1.0f};
    float Intensity = 0.0f;

    DirectionalLightComponent() = default;
    DirectionalLightComponent(const glm::vec3 &radiance, float intensity) : Radiance(radiance), Intensity(intensity)
    {
    }

    void OnInspectorLayout() override
    {
        ImGui::ColorEdit3("Radiance", glm::value_ptr(Radiance));
        ImGui::DragFloat("Intensity", &Intensity, 0.1f, 0.0f, 100.0f);
    }

    void OnDrawGizmos(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) override
    {
        ImU32 color = ImGuiUtils::GetHexColor(ImColor(Radiance.x, Radiance.y, Radiance.z));
        glm::mat4 modelNoScale = RemoveScaling(model);
        ImGuizmo::DrawDirectionalLightGizmos(glm::value_ptr(view), glm::value_ptr(projection),
                                             glm::value_ptr(modelNoScale), 1, color, 0.3f, 1.0f);
    }

    rfl::Generic::Object SerializeToObject() const override
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

    void DeserializeFromObject(const rfl::Generic::Object &object) override
    {
        auto radiance = object.get("Radiance").and_then(rfl::to_object).value();
        Radiance.x = radiance.get("x").and_then(rfl::to_double).value();
        Radiance.y = radiance.get("y").and_then(rfl::to_double).value();
        Radiance.z = radiance.get("z").and_then(rfl::to_double).value();
        Intensity = object.get("Intensity").and_then(rfl::to_double).value();
    }
};

struct PointLightComponent : public BaseComponent
{
    COMPONENT_CLASS_TYPE(PointLight)

    glm::vec3 Radiance{1.0f};
    float Intensity = 1.0f;
    float MinRange = 0.001f;
    float Range = 10.f;

    PointLightComponent() = default;
    PointLightComponent(const glm::vec3 &radiance, float intensity, float minRange, float range)
        : Radiance(radiance), Intensity(intensity), MinRange(minRange), Range(range)
    {
    }

    void OnInspectorLayout() override
    {
        ImGui::ColorEdit3("Radiance", glm::value_ptr(Radiance));
        ImGui::DragFloat("Intensity", &Intensity, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloatRange2("Range", &MinRange, &Range, 0.1f, 0.0f, 100.0f);
    }

    void OnDrawGizmos(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) override
    {
        ImU32 color = ImGuiUtils::GetHexColor(ImColor(Radiance.x, Radiance.y, Radiance.z));
        glm::mat4 modelNoScale = RemoveScaling(model);
        ImGuizmo::DrawPointLightGizmos(glm::value_ptr(view), glm::value_ptr(projection), glm::value_ptr(modelNoScale),
                                       1, color, Range);
    }

    rfl::Generic::Object SerializeToObject() const override
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

    void DeserializeFromObject(const rfl::Generic::Object &object) override
    {
        auto radiance = object.get("Radiance").and_then(rfl::to_object).value();
        Radiance.x = radiance.get("x").and_then(rfl::to_double).value();
        Radiance.y = radiance.get("y").and_then(rfl::to_double).value();
        Radiance.z = radiance.get("z").and_then(rfl::to_double).value();
        Intensity = object.get("Intensity").and_then(rfl::to_double).value();
        MinRange = object.get("MinRange").and_then(rfl::to_double).value();
        Range = object.get("Range").and_then(rfl::to_double).value();
    }
};

struct SpotLightComponent : public BaseComponent
{
    COMPONENT_CLASS_TYPE(SpotLight)

    glm::vec3 Radiance{1.0f};
    float Intensity = 1.0f;
    float MinRange = 0.001f;
    float Range = 10.f;
    float MinAngle = 0.01f;
    float Angle = 30.f;

    SpotLightComponent() = default;
    SpotLightComponent(const glm::vec3 &radiance, float intensity, float minRange, float range, float minAngle,
                       float angle)
        : Radiance(radiance), Intensity(intensity), MinRange(minRange), Range(range), MinAngle(minAngle), Angle(angle)
    {
    }

    void OnInspectorLayout() override
    {
        ImGui::ColorEdit3("Radiance", glm::value_ptr(Radiance));
        ImGui::DragFloat("Intensity", &Intensity, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloatRange2("Range", &MinRange, &Range, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloatRange2("Angle", &MinAngle, &Angle, 0.1f, 0.0f, 180.0f);
    }

    void OnDrawGizmos(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) override
    {
        ImU32 color = ImGuiUtils::GetHexColor(ImColor(Radiance.x, Radiance.y, Radiance.z));
        glm::mat4 modelNoScale = RemoveScaling(model);
        ImGuizmo::DrawSpotLightGizmos(glm::value_ptr(view), glm::value_ptr(projection), glm::value_ptr(model), 1, color,
                                      Range, Angle);
    }

    rfl::Generic::Object SerializeToObject() const override
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

    void DeserializeFromObject(const rfl::Generic::Object &object) override
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
};

struct AreaLightComponent : public BaseComponent
{
    COMPONENT_CLASS_TYPE(AreaLight)

    glm::vec3 Radiance{1.0f};
    float Intensity = 1.0f;
    glm::vec2 Size{1.0f};
    bool TwoSided = false;

    AreaLightComponent() = default;
    AreaLightComponent(const glm::vec3 &radiance, float intensity, const glm::vec2 &size, bool twoSided)
        : Radiance(radiance), Intensity(intensity), Size(size), TwoSided(twoSided)
    {
    }

    void OnInspectorLayout() override
    {
        ImGui::ColorEdit3("Radiance", glm::value_ptr(Radiance));
        ImGui::DragFloat("Intensity", &Intensity, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat2("Size", glm::value_ptr(Size), 0.1f, 0.0f, 100.0f);
        ImGui::Checkbox("Two Sided", &TwoSided);
    }

    void OnDrawGizmos(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) override
    {
        ImU32 color = ImGuiUtils::GetHexColor(ImColor(Radiance.x, Radiance.y, Radiance.z));
        ImGuizmo::DrawAreaLightGizmos(glm::value_ptr(view), glm::value_ptr(projection), glm::value_ptr(model), 1, color,
                                      Size.x, Size.y);
    }

    rfl::Generic::Object SerializeToObject() const override
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

    void DeserializeFromObject(const rfl::Generic::Object &object) override
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
};

} // namespace Doodle
