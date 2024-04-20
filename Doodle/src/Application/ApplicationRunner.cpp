#include "ApplicationRunner.h"
#include "Application.h"

namespace Doodle
{

void ApplicationRunner::Run()
{
    m_app->Run();
    m_app->Deinitialize();
    m_app.reset();
    m_window.reset();
}

} // namespace Doodle