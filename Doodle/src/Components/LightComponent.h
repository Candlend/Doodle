#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "pch.h"
#include <glm/glm.hpp>
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
};

struct PointLightComponent : public BaseComponent
{
    COMPONENT_CLASS_TYPE(PointLight)

    glm::vec3 Radiance{1.0f};
    float Intensity = 1.0f;
    float MinRange = 0.001f;
    float Range = 10.f;

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
};

struct AreaLightComponent : public BaseComponent
{
    COMPONENT_CLASS_TYPE(AreaLight)

    glm::vec3 Radiance{1.0f};
    float Intensity = 1.0f;
    glm::vec2 Size{1.0f};
    bool TwoSided = false;

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
};

} // namespace Doodle