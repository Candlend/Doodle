#include "ApplicationRunner.h"
#include "Application.h"

namespace RhyEngine
{

void ApplicationRunner::Run()
{
    m_app->Run();
    m_app->Deinitialize();
    m_app.reset();
    m_window.reset();
}

} // namespace RhyEngine