#include "Application.h"
#include "Log.h"
#include "Sandbox.h"

int main(int /*argc*/, char ** /*argv*/)
{
    RhyEngine::Log::Init();
    RhyEngine::ApplicationRunner *runner = &RhyEngine::ApplicationRunner::Get();
    runner->CreateApp<Sandbox>();
    runner->Run();
}