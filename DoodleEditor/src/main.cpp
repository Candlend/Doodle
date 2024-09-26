#include "DoodleEditor.h"
#include "EventManager.h"

int main(int /*argc*/, char ** /*argv*/)
{
    Log::Initialize();
    auto runner = Doodle::ApplicationRunner::Get();
    Doodle::WindowProps props{"Doodle", 1920, 1080};
    runner->CreateApp<DoodleEditor>(props);
    runner->Run();
    return 0;
}