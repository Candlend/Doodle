#pragma once

#include <Doodle.h>
#include "ImGuiManager.h"
#include "Renderer.h"
#include "Test.h"


using namespace Doodle;

class Sandbox : public Application
{
public:
    void Initialize() override
    {
        Test::Register();
        Application::Initialize();
        ActivateImGuiContext();
    }
};
