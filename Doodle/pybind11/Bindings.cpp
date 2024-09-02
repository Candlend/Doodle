#define BIND_MODULE(name, ...) void BindModule##name(py::module &m) __VA_ARGS__
#define BIND_SUBMODULE(name, ...) void BindModule##name(py::module &m)

#include "Window.h"
using namespace Doodle;

BIND_MODULE(Window)
{
    py::class_<WindowProps>(m, "WindowProps")
        .def(py::init<const std::string &, unsigned int, unsigned int>(), py::arg("title") = "Doodle",
             py::arg("width") = 1920, py::arg("height") = 1080)
        .def_readwrite("Title", &WindowProps::Title)
        .def_readwrite("Width", &WindowProps::Width)
        .def_readwrite("Height", &WindowProps::Height);

    py::class_<Window, std::shared_ptr<Window>>(m, "Window")
        .def_static("Create", &Window::Create, py::arg("props") = WindowProps(), py::arg("visible") = true)

        .def("PollEvents", &Window::PollEvents)
        .def("SwapBuffers", &Window::SwapBuffers)
        .def("GetWidth", &Window::GetWidth)
        .def("GetHeight", &Window::GetHeight)
        .def("SetVSync", &Window::SetVSync)
        .def("IsVSync", &Window::IsVSync)
        .def("GetNativeWindow", &Window::GetNativeWindow);
}

#include "ApplicationRunner.h"
BIND_MODULE(ApplicationRunner)
{
    py::class_<ApplicationRunner>(m, "ApplicationRunner")
        .def(py::init<>())
        .def_static("Get", &ApplicationRunner::Get, py::return_value_policy::reference)
        .def("CreateApp",
             static_cast<void (ApplicationRunner::*)(py::object, WindowProps)>(&ApplicationRunner::CreateApp),
             py::arg("appClass"), py::arg("props") = WindowProps())
        .def("Run", &ApplicationRunner::Run)
        .def_static("GetWindow", &ApplicationRunner::GetWindow, py::return_value_policy::reference)
        .def_static("GetApplication", &ApplicationRunner::GetApplication, py::return_value_policy::reference)
        .def_static("SetIcon", &ApplicationRunner::SetIcon);
}

#include "Log.h"
BIND_MODULE(Log)
{
    py::enum_<LogType>(m, "LogType")
        .value("Trace", LogType::Trace)
        .value("Debug", LogType::Debug)
        .value("Info", LogType::Info)
        .value("Warning", LogType::Warning)
        .value("Error", LogType::Error)
        .export_values();

    py::class_<LogInfo>(m, "LogInfo")
        .def(py::init<>())
        .def_readwrite("Time", &LogInfo::Time)
        .def_readwrite("Message", &LogInfo::Message)
        .def_readwrite("Stacktrace", &LogInfo::Stacktrace)
        .def_readwrite("Hash", &LogInfo::Hash)
        .def_readwrite("Type", &LogInfo::Type);

    py::class_<CollapsedLogInfo, LogInfo>(m, "CollapsedLogInfo")
        .def(py::init<>())
        .def_readwrite("Count", &CollapsedLogInfo::Count);

    py::class_<Log>(m, "Log")
        .def_static("Initialize", &Log::Initialize)
        .def_static("LoadConfig", &Log::LoadConfig)
        .def_static("GetCoreLogger", &Log::GetCoreLogger, py::return_value_policy::reference)
        .def_static("GetClientLogger", &Log::GetClientLogger, py::return_value_policy::reference)
        .def_static("GetLogInfos", &Log::GetLogInfos, py::return_value_policy::reference)
        .def_static("GetCollapsedLogInfos", &Log::GetCollapsedLogInfos, py::return_value_policy::reference)
        .def_static("GetLogCount", &Log::GetLogCount)
        .def_static("Clear", &Log::Clear);

    // 暴露宏定义，并使用 fmt::format 进行字符串格式化
    m.def("DOO_CORE_TRACE", [](const std::string &msg) { DOO_CORE_TRACE(fmt::format("{}", msg)); });
    m.def("DOO_CORE_DEBUG", [](const std::string &msg) { DOO_CORE_DEBUG(fmt::format("{}", msg)); });
    m.def("DOO_CORE_INFO", [](const std::string &msg) { DOO_CORE_INFO(fmt::format("{}", msg)); });
    m.def("DOO_CORE_WARN", [](const std::string &msg) { DOO_CORE_WARN(fmt::format("{}", msg)); });
    m.def("DOO_CORE_ERROR", [](const std::string &msg) { DOO_CORE_ERROR(fmt::format("{}", msg)); });

    m.def("DOO_TRACE", [](const std::string &msg) { DOO_TRACE(fmt::format("{}", msg)); });
    m.def("DOO_DEBUG", [](const std::string &msg) { DOO_DEBUG(fmt::format("{}", msg)); });
    m.def("DOO_INFO", [](const std::string &msg) { DOO_INFO(fmt::format("{}", msg)); });
    m.def("DOO_WARN", [](const std::string &msg) { DOO_WARN(fmt::format("{}", msg)); });
    m.def("DOO_ERROR", [](const std::string &msg) { DOO_ERROR(fmt::format("{}", msg)); });
}

#include "PyApplication.h"
BIND_MODULE(Application)
{
    py::class_<Application::Time>(m, "Time")
        .def_static("GetTime", &Application::Time::GetTime)
        .def_static("GetDeltaTime", &Application::Time::GetDeltaTime)
        .def_static("GetFPS", &Application::Time::GetFPS);

    py::class_<Application, PyApplication, std::shared_ptr<Application>>(m, "Application")
        .def(py::init<>())
        .def("Initialize", &Application::Initialize)
        .def("Deinitialize", &Application::Deinitialize)
        .def("BeforeUpdate", &Application::BeforeUpdate)
        .def("AfterUpdate", &Application::AfterUpdate)
        .def("BeforeLayout", &Application::BeforeLayout)
        .def("AfterLayout", &Application::AfterLayout)
        .def("BeforeRender", &Application::BeforeRender)
        .def("AfterRender", &Application::AfterRender);
}

std::vector<std::function<void(py::module &)>> g_Bindings = {BindModuleWindow, BindModuleApplicationRunner,
                                                             BindModuleLog, BindModuleApplication};

PYBIND11_MODULE(doodle, m)
{
    for (auto &bind : g_Bindings)
    {
        bind(m);
    }
}