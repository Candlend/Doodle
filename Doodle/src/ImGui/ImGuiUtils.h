#pragma once

#include "pch.h"
#include <imgui.h>

#include "ImGuiUtils.Feature.h"
#include "ImGuiUtils.Widget.h"

namespace Doodle
{
namespace ImGuiUtils
{

inline bool ToggleButton(const char *strId, bool &v, const ImVec2 &size = ImVec2(0, 0))
{
    // 获取当前按钮的活动颜色
    ImVec4 color = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
    ImVec4 defaultColor = ImGui::GetStyle().Colors[ImGuiCol_Button];

    // 使用颜色样式
    ImGuiUtils::StyleColorScope sc{ImGuiCol_Button, v ? color : defaultColor}; // 使用白色作为默认颜色

    // 创建按钮
    if (ImGui::Button(strId, size))
    {
        v = !v;                 // 切换状态
        ImGui::PopStyleColor(); // 恢复样式
        return true;            // 返回按钮被点击
    }

    return false; // 返回按钮未被点击
}

inline ImColor GetColor(ImU32 hex)
{
    if (hex <= 0xFFFFFF)
    {
        hex = hex << 8 | 0xFF; // 如果没有透明度，添加透明度
    }
    // 提取 RGBA 值
    auto r = (hex >> 24) & 0xFF; // 提取红色
    auto g = (hex >> 16) & 0xFF; // 提取绿色
    auto b = (hex >> 8) & 0xFF;  // 提取蓝色
    auto a = hex & 0xFF;         // 提取透明度
    return IM_COL32(r, g, b, a);
}

inline bool InputText(const char *label, std::string &str, ImGuiInputTextFlags flags = 0)
{
    // TODO 好像无法编辑
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "%s", str.c_str());
    if (ImGui::InputText(label, buffer, sizeof(buffer), flags))
    {
        str = buffer;
        return true;
    }
    return false;
}

inline bool SizedButton(const char *label, int n = 1)
{
    auto style = ImGui::GetStyle();
    float buttonWidth = (ImGui::GetWindowSize().x - style.WindowPadding.x * 2 - style.ItemSpacing.x * (n - 1)) / n;
    return ImGui::Button(label, ImVec2(buttonWidth, 0));
}

inline void ReadOnlyInputText(const char *label, std::string str)
{
    ImGuiUtils::StyleColorScope sc{ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]};
    ImGui::BeginDisabled();
    ImGui::InputText(label, const_cast<char *>(str.c_str()), str.size(), ImGuiInputTextFlags_ReadOnly);
    ImGui::EndDisabled();
}

template <typename... Args>
inline void ReadOnlyInputText(const char *label, fmt::format_string<Args...> format, Args &&...args)
{
    // DisabledScope ds;
    std::string str = fmt::format(format, std::forward<Args>(args)...);
    std::vector<char> buffer(str.begin(), str.end());
    buffer.push_back('\0');
    ImGuiUtils::StyleColorScope sc{ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]};
    ImGui::BeginDisabled();
    ImGui::InputText(label, buffer.data(), buffer.size(), ImGuiInputTextFlags_ReadOnly);
    ImGui::EndDisabled();
}

inline void ReadOnlyInputFloat(const char *label, float value)
{
    ImGuiUtils::StyleColorScope sc{ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]};
    ImGui::BeginDisabled();
    ImGui::DragFloat(label, &value);
    ImGui::EndDisabled();
}

inline void ReadOnlyInputInt(const char *label, int value)
{
    ImGuiUtils::StyleColorScope sc{ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]};
    ImGui::BeginDisabled();
    ImGui::DragInt(label, &value);
    ImGui::EndDisabled();
}

} // namespace ImGuiUtils
} // namespace Doodle