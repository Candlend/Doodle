#include "ApplicationRunner.h"
#include "Application.h"

namespace RhyEngine
{

void ApplicationRunner::Run()
{
    m_app->Run();
}

Application &ApplicationRunner::GetCurrentApp()
{
    return *m_app;
}

} // namespace RhyEngine