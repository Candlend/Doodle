#pragma once

#include "RhyEngine/Application.h"
#include "RhyEngine/Log.h"

extern RhyEngine::Application *RhyEngine::CreateApplication();

int main(int /*argc*/, char ** /*argv*/)
{
    RhyEngine::Log::Init();
    RHY_CORE_WARN("Initialized Log!");
    int a = 5;
    RHY_INFO("Hello! Var={0}", a);
    auto *app = RhyEngine::CreateApplication();
    app->Run();
    delete app;
}
