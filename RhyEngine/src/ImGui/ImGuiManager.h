#pragma once

#include "Singleton.h"

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

    float m_time = 0.0f;
};

} // namespace RhyEngine