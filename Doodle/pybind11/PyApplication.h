#include "Application.h"
#include "pch.h"

namespace Doodle
{

class DOO_API PyApplication : public Application
{
public:
    using Application::Application;

    void Initialize() override
    {
        PYBIND11_OVERRIDE(void, Application, Initialize, );
    }
    void Deinitialize() override
    {
        PYBIND11_OVERRIDE(void, Application, Deinitialize, );
    }
    void BeforeUpdate() override
    {
        PYBIND11_OVERRIDE(void, Application, BeforeUpdate, );
    }
    void AfterUpdate() override
    {
        PYBIND11_OVERRIDE(void, Application, AfterUpdate, );
    }
    void BeforeLayout() override
    {
        PYBIND11_OVERRIDE(void, Application, BeforeLayout, );
    }
    void AfterLayout() override
    {
        PYBIND11_OVERRIDE(void, Application, AfterLayout, );
    }
    void BeforeRender() override
    {
        PYBIND11_OVERRIDE(void, Application, BeforeRender, );
    }
    void AfterRender() override
    {
        PYBIND11_OVERRIDE(void, Application, AfterRender, );
    }
};

} // namespace Doodle