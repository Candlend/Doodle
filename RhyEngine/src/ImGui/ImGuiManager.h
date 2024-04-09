#pragma once

#include "Singleton.h"
#include "ApplicationEvent.h"

namespace RhyEngine
{

class RHY_API ImGuiManager : public Singleton<ImGuiManager>
{
public:
    void Initialize();
    void Deinitialize();
    void DrawLayout();

protected:
    void BeginFrame();
    void EndFrame();
};

} // namespace RhyEngine