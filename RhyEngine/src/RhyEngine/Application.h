#pragma once

#include "Core.h"
#include "Common/Singleton.h"

namespace RhyEngine
{

class RHY_API Application : Singleton<Application>
{
public:
    Application() = default;

    virtual ~Application() = default;

    void Run();
};

// To be defined in CLIENT
Application *CreateApplication();

} // namespace RhyEngine