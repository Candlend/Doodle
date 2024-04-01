#ifdef RHY_PLATFORM_WINDOWS

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "ApplicationEvent.h"
#include "KeyCode.h"
#include "KeyEvent.h"
#include "MouseEvent.h"
#include "Window.h"


namespace RhyEngine
{

class WindowsWindow : public Window
{
public:
    explicit WindowsWindow(const WindowProps &props)
    {
        Init(props);
    }
    virtual ~WindowsWindow()
    {
        Shutdown();
    }

    void OnUpdate() override
    {
        glfwPollEvents();
        glfwSwapBuffers(m_window);
    }

    inline unsigned int GetWidth() const override
    {
        return m_data.Width;
    }
    inline unsigned int GetHeight() const override
    {
        return m_data.Height;
    }

    void SetEventCallback(const EventCallbackFn &callback) override
    {
        m_data.EventCallbackFn = callback;
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
    inline virtual void *GetNativeWindow() const override
    {
        return m_window;
    }

private:
    struct WindowData
    {
        std::string Title;
        unsigned int Width, Height;
        bool VSync;

        EventCallbackFn EventCallbackFn;
        std::unordered_map<KeyCode, int> KeyRepeatCounts;
    };

    void Init(const WindowProps &props)
    {
        m_data.Title = props.Title;
        m_data.Width = props.Width;
        m_data.Height = props.Height;

        RHY_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

        static bool s_GLFWInitialized = false;
        if (!s_GLFWInitialized)
        {
            int success = glfwInit();
            RHY_CORE_ASSERT(success, "Could not initialize GLFW!");
            glfwSetErrorCallback(GLFWErrorCallback);
            s_GLFWInitialized = true;
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
            data.EventCallbackFn(event);
        });

        glfwSetWindowCloseCallback(m_window, [](GLFWwindow *window) {
            WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
            WindowCloseEvent event;
            data.EventCallbackFn(event);
        });

        glfwSetKeyCallback(m_window, [](GLFWwindow *window, int key, int /*scancode*/, int action, int /*mods*/) {
            WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
            KeyCode keycode = static_cast<KeyCode>(key);
            switch (action)
            {
            case GLFW_PRESS: {
                KeyPressedEvent event(keycode, 0);
                data.EventCallbackFn(event);
                data.KeyRepeatCounts[keycode] = 0; // 初始化重复计数
                break;
            }
            case GLFW_RELEASE: {
                KeyReleasedEvent event(keycode);
                data.EventCallbackFn(event);
                data.KeyRepeatCounts.erase(keycode); // 移除按键的重复计数
                break;
            }
            case GLFW_REPEAT: {
                data.KeyRepeatCounts[keycode]++; // 增加重复计数
                KeyPressedEvent event(keycode, data.KeyRepeatCounts[keycode]);
                data.EventCallbackFn(event);
                break;
            }
            }
        });

        glfwSetCharCallback(m_window, [](GLFWwindow *window, unsigned int keycode) {
            WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
            CharInputEvent event(static_cast<KeyCode>(keycode));
            data.EventCallbackFn(event);
        });

        glfwSetMouseButtonCallback(m_window, [](GLFWwindow *window, int button, int action, int /*mods*/) {
            WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
            MouseButtonCode btn = static_cast<MouseButtonCode>(button);
            switch (action)
            {
            case GLFW_PRESS: {
                MouseButtonPressedEvent event(btn);
                data.EventCallbackFn(event);
                break;
            }
            case GLFW_RELEASE: {
                MouseButtonReleasedEvent event(btn);
                data.EventCallbackFn(event);
                break;
            }
            }
        });

        glfwSetScrollCallback(m_window, [](GLFWwindow *window, double xOffset, double yOffset) {
            WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
            MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
            data.EventCallbackFn(event);
        });

        glfwSetCursorPosCallback(m_window, [](GLFWwindow *window, double xPos, double yPos) {
            WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
            MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
            data.EventCallbackFn(event);
        });
    }

    void Shutdown()
    {
        glfwDestroyWindow(m_window);
    }

    GLFWwindow *m_window;
    WindowData m_data;

    static void GLFWErrorCallback(int error, const char *description)
    {
        RHY_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
    }
};

std::unique_ptr<Window> Window::Create(const WindowProps &props)
{
    return std::make_unique<WindowsWindow>(props);
}

} // namespace RhyEngine

#endif