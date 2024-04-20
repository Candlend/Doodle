#include "Sandbox.h"


int main(int /*argc*/, char ** /*argv*/)
{
    Doodle::Log::Init();
    Doodle::ApplicationRunner &runner = Doodle::ApplicationRunner::Get();
    Doodle::WindowProps props{"Doodle", 1280, 720};
    runner.CreateApp<Sandbox>(props);
    runner.Run();
    DOO_INFO("Quitting...");
    return 0;
}