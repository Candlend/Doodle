#pragma once

#include "Core.h"

namespace RhyEngine
{

class RHY_API Application
{
public:
    Application();
    virtual ~Application();

    void Run();
};

// To be defined in CLIENT
Application *CreateApplication();

} // namespace RhyEngine