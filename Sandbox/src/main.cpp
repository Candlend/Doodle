#include "Sandbox.h"


int main(int /*argc*/, char ** /*argv*/)
{
    RhyEngine::Log::Init();
    RhyEngine::ApplicationRunner &runner = RhyEngine::ApplicationRunner::Get();
    RhyEngine::WindowProps props{"Sandbox", 1280, 720};
    runner.CreateApp<Sandbox>(props);
    runner.Run();
    RHY_INFO("Quitting...");
    return 0;
}