#include "EventManager.h"
#include "Sandbox.h"


int main(int /*argc*/, char ** /*argv*/)
{
    Log::Initialize();
    auto *runner = Doodle::ApplicationRunner::Get();
    Doodle::WindowProps props{"Doodle", 1920, 1080};
    runner->CreateApp<Sandbox>(props);
    runner->Run();
    return 0;
}