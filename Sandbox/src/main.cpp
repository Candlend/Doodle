#include "ApplicationRunner.h"
#include "GLFW/glfw3.h"
#include "Log.h"
#include "Sandbox.h"
#include "Window.h"


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