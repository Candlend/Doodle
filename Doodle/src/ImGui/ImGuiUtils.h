#pragma once

#include <imgui.h>

#include "ImGuiUtils.Feature.h"
#include "ImGuiUtils.Widget.h"

namespace Doodle
{
namespace ImGuiUtils
{

bool ToggleButton(const char *strId, bool &v, const ImVec2 &size = ImVec2(0, 0))
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

ImColor GetColor(ImU32 hex)
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

} // namespace ImGuiUtils
} // namespace Doodle