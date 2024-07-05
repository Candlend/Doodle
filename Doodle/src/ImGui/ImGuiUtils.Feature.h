#pragma once

#include <imgui.h>

namespace Doodle
{
namespace ImGuiUtils
{
class StyleVarScope
{
public:
    explicit StyleVarScope(ImGuiStyleVar idx, float val)
    {
        ImGui::PushStyleVar(idx, val);
    }

    explicit StyleVarScope(ImGuiStyleVar idx, const ImVec2 &val)
    {
        ImGui::PushStyleVar(idx, val);
    }

    ~StyleVarScope()
    {
        ImGui::PopStyleVar();
    }
};

class StyleColorScope
{
public:
    explicit StyleColorScope(ImGuiCol idx, ImU32 col)
    {
        ImGui::PushStyleColor(idx, col);
    }

    explicit StyleColorScope(ImGuiCol idx, const ImVec4 &col)
    {
        ImGui::PushStyleColor(idx, col);
    }

    explicit StyleColorScope(ImGuiCol idx, const ImColor &col)
    {
        ImGui::PushStyleColor(idx, col.Value);
    }

    ~StyleColorScope()
    {
        ImGui::PopStyleColor();
    }
};

class ItemWidthScope
{
public:
    explicit ItemWidthScope(float itemWidth)
    {
        ImGui::PushItemWidth(itemWidth);
    }

    ~ItemWidthScope()
    {
        ImGui::PopItemWidth();
    }
};

class IDScope
{
public:
    explicit IDScope(const char *strId)
    {
        ImGui::PushID(strId);
    }

    explicit IDScope(int intId)
    {
        ImGui::PushID(intId);
    }

    explicit IDScope(void *ptrId)
    {
        ImGui::PushID(ptrId);
    }

    ~IDScope()
    {
        ImGui::PopID();
    }
};

class FontScope
{
public:
    explicit FontScope(ImFont *font)
    {
        ImGui::PushFont(font);
    }

    ~FontScope()
    {
        ImGui::PopFont();
    }
};

class ButtonRepeatScope
{
public:
    explicit ButtonRepeatScope(bool repeat)
    {
        ImGui::PushButtonRepeat(repeat);
    }

    ~ButtonRepeatScope()
    {
        ImGui::PopButtonRepeat();
    }
};

class ClipRectScope
{
public:
    explicit ClipRectScope(const ImVec2 &clipRectMin, const ImVec2 &clipRectMax, bool intersectWithCurrentClipRect)
    {
        ImGui::PushClipRect(clipRectMin, clipRectMax, intersectWithCurrentClipRect);
    }

    ~ClipRectScope()
    {
        ImGui::PopClipRect();
    }
};

class TextWrapPosScope
{
public:
    explicit TextWrapPosScope(float wrapLocalPosX)
    {
        ImGui::PushTextWrapPos(wrapLocalPosX);
    }

    ~TextWrapPosScope()
    {
        ImGui::PopTextWrapPos();
    }
};

} // namespace ImGuiUtils
} // namespace Doodle