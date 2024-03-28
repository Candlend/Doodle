#include "Application.h"
#include "GLFW/glfw3.h"
#include "Log.h"
#include "Sandbox.h"

int main(int /*argc*/, char ** /*argv*/)
{
    RhyEngine::Log::Init();
    RhyEngine::ApplicationRunner &runner = RhyEngine::ApplicationRunner::Get();
    runner.CreateApp<Sandbox>();
    runner.Run();
    RHY_INFO("Quitting...");
    return 0;
}