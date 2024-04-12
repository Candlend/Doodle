#pragma once

#include "ApplicationEvent.h"
#include "Singleton.h"
#include "imgui.h"


namespace RhyEngine
{

class RHY_API ImGuiManager : public Singleton<ImGuiManager>
{
public:
    void Initialize();
    void Deinitialize();
    void DrawLayout();

    inline ImGuiContext *GetContext()
    {
        return m_context;
    }

protected:
    void BeginFrame();
    void EndFrame();

    ImGuiContext *m_context;
};

#ifndef RHY_BUILD_DLL
void ActivateImGuiContext()
{
    ImGui::SetCurrentContext(ImGuiManager::Get().GetContext());
}
#endif

} // namespace RhyEngine