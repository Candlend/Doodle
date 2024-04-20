#pragma once

#include "ApplicationEvent.h"
#include "Singleton.h"
#include "imgui.h"


namespace Doodle
{

class DOO_API ImGuiManager : public Singleton<ImGuiManager>
{
public:
    void Initialize();
    void Deinitialize();
    void DrawLayout();

    inline ImGuiContext *GetContext()
    {
        return m_context;
    }

    void RegisterFont(int sizeInPixels, std::string englishFont, std::string chineseFont, std::string iconFont, std::string brandFont);

protected:
    void BeginFrame();
    void EndFrame();
    void ShowDockspace();

    ImGuiContext *m_context;
    std::vector<ImFont *> m_fonts;
};

#ifndef DOO_BUILD_DLL
void ActivateImGuiContext()
{
    ImGui::SetCurrentContext(ImGuiManager::Get().GetContext());
}
#endif

} // namespace Doodle