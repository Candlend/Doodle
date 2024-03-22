#include "pch.h"

#include "RhyEngine/Event/ApplicationEvent.h"
#include "RhyEngine/Event/KeyEvent.h"
#include "RhyEngine/Event/MouseEvent.h"
#include "WindowsWindow.h"
#include <glad/glad.h>

namespace RhyEngine
{

static bool g_GLFWInitialized = false;

static void GLFWErrorCallback(int error, const char *description)
{
    RHY_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
}

Window *Window::Create(const WindowProps &props)
{
    return new WindowsWindow(props);
}

WindowsWindow::WindowsWindow(const WindowProps &props)
{
    Init(props);
}

WindowsWindow::~WindowsWindow()
{
    Shutdown();
}

void WindowsWindow::Init(const WindowProps &props)
{
    m_data.Title = props.Title;
    m_data.Width = props.Width;
    m_data.Height = props.Height;

    RHY_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

    if (!g_GLFWInitialized)
    {
        // TODO: glfwTerminate on system shutdown
        int success = glfwInit();
        RHY_CORE_ASSERT(success, "Could not intialize GLFW!");
        glfwSetErrorCallback(GLFWErrorCallback);
        g_GLFWInitialized = true;
    }

    m_window = glfwCreateWindow(static_cast<int>(props.Width), static_cast<int>(props.Height), m_data.Title.c_str(),
                                nullptr, nullptr);
    glfwMakeContextCurrent(m_window);
    int status = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
    RHY_CORE_ASSERT(status, "Failed to initialize Glad!");
    glfwSetWindowUserPointer(m_window, &m_data);
    SetVSync(true);

    // Set GLFW callbacks
    glfwSetWindowSizeCallback(m_window, [](GLFWwindow *window, int width, int height) {
        WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
        data.Width = width;
        data.Height = height;

        WindowResizeEvent event(width, height);
        data.EventCallback(event);
    });

    glfwSetWindowCloseCallback(m_window, [](GLFWwindow *window) {
        WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
        WindowCloseEvent event;
        data.EventCallback(event);
    });

    glfwSetKeyCallback(m_window, [](GLFWwindow *window, int key, int /*scancode*/, int action, int /*mods*/) {
        WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

        switch (action)
        {
        case GLFW_PRESS: {
            KeyPressedEvent event(key, 0);
            data.EventCallback(event);
            data.KeyRepeatCounts[key] = 0; // 初始化重复计数
            break;
        }
        case GLFW_RELEASE: {
            KeyReleasedEvent event(key);
            data.EventCallback(event);
            data.KeyRepeatCounts.erase(key); // 移除按键的重复计数
            break;
        }
        case GLFW_REPEAT: {
            data.KeyRepeatCounts[key]++; // 增加重复计数
            KeyPressedEvent event(key, data.KeyRepeatCounts[key]);
            data.EventCallback(event);
            break;
        }
        }
    });

    glfwSetMouseButtonCallback(m_window, [](GLFWwindow *window, int button, int action, int /*mods*/) {
        WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

        switch (action)
        {
        case GLFW_PRESS: {
            MouseButtonPressedEvent event(button);
            data.EventCallback(event);
            break;
        }
        case GLFW_RELEASE: {
            MouseButtonReleasedEvent event(button);
            data.EventCallback(event);
            break;
        }
        }
    });

    glfwSetScrollCallback(m_window, [](GLFWwindow *window, double xOffset, double yOffset) {
        WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

        MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
        data.EventCallback(event);
    });

    glfwSetCursorPosCallback(m_window, [](GLFWwindow *window, double xPos, double yPos) {
        WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

        MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
        data.EventCallback(event);
    });
}

void WindowsWindow::Shutdown()
{
    glfwDestroyWindow(m_window);
}

void WindowsWindow::OnUpdate()
{
    glfwPollEvents();
    glfwSwapBuffers(m_window);
}

void WindowsWindow::SetVSync(bool enabled)
{
    if (enabled)
        glfwSwapInterval(1);
    else
        glfwSwapInterval(0);

    m_data.VSync = enabled;
}

bool WindowsWindow::IsVSync() const
{
    return m_data.VSync;
}

} // namespace RhyEngine
