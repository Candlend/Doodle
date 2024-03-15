#pragma once

#include "RhyEngine/Application.h"
#include "RhyEngine/Log.h"

extern RhyEngine::Application *RhyEngine::CreateApplication();

int main(int /*argc*/, char ** /*argv*/)
{
    RhyEngine::Log::Init();
    auto *pApp = RhyEngine::CreateApplication();
    pApp->Run();
    delete pApp;
}
