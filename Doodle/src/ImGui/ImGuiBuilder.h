#pragma once

#include <imgui.h>

#include <ImGuizmo.h>
#include <imnodes.h>

#include "ApplicationEvent.h"
#include "Singleton.h"

namespace Doodle
{

class DOO_API ImGuiBuilder : public Singleton<ImGuiBuilder>
{
public:
    void Initialize();
    void Deinitialize();

    inline ImGuiContext *GetContext()
    {
        return m_imguiContext;
    }

    inline ImNodesContext *GetImnodesContext()
    {
        return m_imnodesContext;
    }

    void RegisterFont(int sizeInPixels, std::string englishFont, std::string chineseFont, std::string iconFont,
                      std::string brandFont);

protected:
    void BeginFrame();
    void EndFrame();

    ImNodesContext *m_imnodesContext;
    ImGuiContext *m_imguiContext;
    std::vector<ImFont *> m_fonts;
};

#ifndef DOO_BUILD_DLL
void ActivateImGuiContext()
{
    ImNodes::SetCurrentContext(ImGuiBuilder::Get()->GetImnodesContext());
    ImGui::SetCurrentContext(ImGuiBuilder::Get()->GetContext());
}
#endif

} // namespace Doodle