#pragma once

#include "RhyEngine/Application.h"
#include "RhyEngine/Log.h"

int main(int /*argc*/, char ** /*argv*/)
{
    RhyEngine::Log::Init();
    auto *app = RhyEngine::CreateApplication();
    app->Run();
    delete app;
}
