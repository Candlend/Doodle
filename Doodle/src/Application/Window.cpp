
#ifdef DOO_PLATFORM_WINDOWS

#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <glad/glad.h>

#include "Application.h"
#include "ApplicationEvent.h"
#include "EventManager.h"
#include "GraphicsContext.h" // Add this line to include the header file for GraphicsContext
#include "KeyCode.h"
#include "KeyEvent.h"
#include "MouseEvent.h"
#include "Window.h"

namespace Doodle
{

class WindowsWindow : public Window
{
public:
    WindowsWindow(const WindowProps &props, bool visible)
    {
        Initialize(props, visible);
    }

    ~WindowsWindow()
    {
        Shutdown();
    }

    void PollEvents() override
    {
        glfwPollEvents();
    }

    void SwapBuffers() override
    {
        m_context->SwapBuffers();
    }

    unsigned int GetWidth() const override
    {
        return m_data.Width;
    }
    unsigned int GetHeight() const override
    {
        return m_data.Height;
    }

    void SetVSync(bool enabled) override
    {
        if (enabled)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);
        m_data.VSync = enabled;
    }
    bool IsVSync() const override
    {
        return m_data.VSync;
    }
    void *GetNativeWindow() const override
    {
        return m_window;
    }

private:
    struct WindowData
    {
        std::string Title;
        unsigned int Width, Height;
        bool VSync;
    };

    void Initialize(const WindowProps &props, bool visible)
    {
        m_data.Title = props.Title;
        m_data.Width = props.Width;
        m_data.Height = props.Height;

        DOO_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

        static bool s_GLFWInitialized = false;
        if (!s_GLFWInitialized)
        {
            int success = glfwInit();
            DOO_CORE_ASSERT(success, "Could not initialize GLFW!");
            glfwSetErrorCallback(GLFWErrorCallback);
            s_GLFWInitialized = true;
        }
        if (!visible)
        {
            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        }
        m_window = glfwCreateWindow(static_cast<int>(props.Width), static_cast<int>(props.Height), m_data.Title.c_str(),
                                    nullptr, nullptr);
        m_context = GraphicsContext::Create(m_window);
        m_context->Initialize();

        glfwSetWindowUserPointer(m_window, this);
        SetVSync(true);

        // Set GLFW callbacks
        glfwSetWindowSizeCallback(m_window, [](GLFWwindow *window, int width, int height) {
            WindowsWindow &appWindow = *static_cast<WindowsWindow *>(glfwGetWindowUserPointer(window));
            appWindow.m_data.Width = width;
            appWindow.m_data.Height = height;

            EventManager::Get()->Dispatch<WindowResizeEvent>(width, height);
        });

        glfwSetWindowRefreshCallback(m_window, [](GLFWwindow *window) {
            WindowsWindow &appWindow = *static_cast<WindowsWindow *>(glfwGetWindowUserPointer(window));

            EventManager::Get()->Dispatch<WindowRefreshEvent>();
        });

        glfwSetWindowPosCallback(m_window, [](GLFWwindow *window, int xPos, int yPos) {
            WindowsWindow &appWindow = *static_cast<WindowsWindow *>(glfwGetWindowUserPointer(window));

            EventManager::Get()->Dispatch<WindowMoveEvent>(xPos, yPos);
        });

        glfwSetWindowCloseCallback(m_window,
                                   [](GLFWwindow * /*window*/) { EventManager::Get()->Dispatch<WindowCloseEvent>(); });
    }

    void Shutdown()
    {
        glfwDestroyWindow(m_window);
    }

    GLFWwindow *m_window;
    GraphicsContext *m_context;
    WindowData m_data;

    static void GLFWErrorCallback(int error, const char *description)
    {
        DOO_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
    }
};

std::shared_ptr<Window> Window::Create(const WindowProps &props, bool visible)
{
    return std::make_shared<WindowsWindow>(props, visible);
}

} // namespace Doodle

#endif
