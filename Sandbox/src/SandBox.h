#pragma once

#include "LogWindow.h"
#include "Test.h"
#include <Doodle.h>

using namespace Doodle;

class Sandbox : public Application
{
public:
    void Initialize() override
    {
        LogWindow::Register();
        Test::Register();
        Application::Initialize();
        ActivateImGuiContext();
    }
};
