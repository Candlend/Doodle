#include "ApplicationRunner.h"
#include "Application.h"

namespace RhyEngine
{

void ApplicationRunner::Run()
{
    m_app->Run();
    m_app.reset();
}

Application &ApplicationRunner::GetCurrentApp()
{
    return *m_app;
}

} // namespace RhyEngine