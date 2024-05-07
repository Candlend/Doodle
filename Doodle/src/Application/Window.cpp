
#ifdef DOO_PLATFORM_WINDOWS

#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Application.h"
#include "ApplicationEvent.h"
#include "EventManager.h"
#include "KeyCode.h"
#include "KeyEvent.h"
#include "MouseEvent.h"
#include "Window.h"

namespace Doodle
{

class Window::Impl
{
public:
    explicit Impl(const WindowProps &props, bool visible)
    {
        Initialize(props, visible);
    }

    ~Impl()
    {
        Shutdown();
    }

    void BeginFrame()
    {
        glfwPollEvents();
    }

    void EndFrame()
    {
        glfwSwapBuffers(m_window);
    }

    unsigned int GetWidth() const
    {
        return m_data.Width;
    }
    unsigned int GetHeight() const
    {
        return m_data.Height;
    }

    void SetVSync(bool enabled)
    {
        if (enabled)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);
        m_data.VSync = enabled;
    }
    bool IsVSync() const
    {
        return m_data.VSync;
    }
    void *GetNativeWindow() const
    {
        return m_window;
    }

private:
    EventManager m_eventManager;
    std::unordered_map<KeyCode, int> m_keyRepeatCounts;

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
        glfwMakeContextCurrent(m_window);
        int status = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
        DOO_CORE_ASSERT(status, "Failed to initialize Glad!");
        glfwSetWindowUserPointer(m_window, this);
        SetVSync(true);

        // 加载图标
        int iconWidthSmall, iconHeightSmall, iconChannelsSmall;
        int iconWidthLarge, iconHeightLarge, iconChannelsLarge;

        // 加载小图标
        stbi_uc *iconPixelsSmall =
            stbi_load("assets/icons/icon_small.png", &iconWidthSmall, &iconHeightSmall, &iconChannelsSmall, 4);
        if (!iconPixelsSmall)
        {
            DOO_CORE_WARN("Failed to load small window icon!");
        }

        // 加载大图标
        stbi_uc *iconPixelsLarge =
            stbi_load("assets/icons/icon_large.png", &iconWidthLarge, &iconHeightLarge, &iconChannelsLarge, 4);
        if (!iconPixelsLarge)
        {
            DOO_CORE_WARN("Failed to load large window icon!");
        }

        // 设置图标
        GLFWimage images[2];
        if (iconPixelsSmall && iconPixelsLarge)
        {
            images[0].width = iconWidthSmall;
            images[0].height = iconHeightSmall;
            images[0].pixels = iconPixelsSmall;

            images[1].width = iconWidthLarge;
            images[1].height = iconHeightLarge;
            images[1].pixels = iconPixelsLarge;

            glfwSetWindowIcon(m_window, 2, images);
        }

        // 释放内存
        if (iconPixelsSmall)
        {
            stbi_image_free(iconPixelsSmall);
        }
        if (iconPixelsLarge)
        {
            stbi_image_free(iconPixelsLarge);
        }

        // Set GLFW callbacks
        glfwSetWindowSizeCallback(m_window, [](GLFWwindow *window, int width, int height) {
            Window::Impl &windowImpl = *static_cast<Window::Impl *>(glfwGetWindowUserPointer(window));
            windowImpl.m_data.Width = width;
            windowImpl.m_data.Height = height;

            WindowResizeEvent event(width, height);
            EventManager::Get().Dispatch(event);
        });

        glfwSetWindowCloseCallback(m_window, [](GLFWwindow * /*window*/) {
            WindowCloseEvent event;
            EventManager::Get().Dispatch(event);
        });

        glfwSetKeyCallback(m_window, [](GLFWwindow *window, int key, int /*scancode*/, int action, int /*mods*/) {
            Window::Impl &windowImpl = *static_cast<Window::Impl *>(glfwGetWindowUserPointer(window));
            KeyCode keycode = static_cast<KeyCode>(key);
            switch (action)
            {
            case GLFW_PRESS: {
                KeyPressedEvent event(keycode, 0);
                EventManager::Get().Dispatch(event); // 初始化重复计数
                break;
            }
            case GLFW_RELEASE: {
                KeyReleasedEvent event(keycode);
                EventManager::Get().Dispatch(event);
                windowImpl.m_keyRepeatCounts.erase(keycode); // 移除按键的重复计数
                break;
            }
            case GLFW_REPEAT: {
                windowImpl.m_keyRepeatCounts[keycode]++; // 增加重复计数
                KeyPressedEvent event(keycode, windowImpl.m_keyRepeatCounts[keycode]);
                EventManager::Get().Dispatch(event);
                break;
            }
            }
        });

        glfwSetCharCallback(m_window, [](GLFWwindow * /*window*/, unsigned int keycode) {
            CharInputEvent event(static_cast<KeyCode>(keycode));
            EventManager::Get().Dispatch(event);
        });

        glfwSetMouseButtonCallback(m_window, [](GLFWwindow * /*window*/, int button, int action, int /*mods*/) {
            MouseButtonCode btn = static_cast<MouseButtonCode>(button);
            switch (action)
            {
            case GLFW_PRESS: {
                MouseButtonPressedEvent event(btn);
                EventManager::Get().Dispatch(event);
                break;
            }
            case GLFW_RELEASE: {
                MouseButtonReleasedEvent event(btn);
                EventManager::Get().Dispatch(event);
                break;
            }
            }
        });

        glfwSetScrollCallback(m_window, [](GLFWwindow * /*window*/, double xOffset, double yOffset) {
            MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
            EventManager::Get().Dispatch(event);
        });

        glfwSetCursorPosCallback(m_window, [](GLFWwindow * /*window*/, double xPos, double yPos) {
            MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
            EventManager::Get().Dispatch(event);
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
        DOO_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
    }
};

std::unique_ptr<Window> Window::Create(const WindowProps &props, bool visible)
{
    return std::make_unique<Window>(props, visible);
}

Window::Window(const WindowProps &props, bool visible) : m_impl(std::make_unique<Impl>(props, visible))
{
}

Window::~Window() = default;

void Window::BeginFrame()
{
    m_impl->BeginFrame();
}
void Window::EndFrame()
{
    m_impl->EndFrame();
}
unsigned int Window::GetWidth() const
{
    return m_impl->GetWidth();
}
unsigned int Window::GetHeight() const
{
    return m_impl->GetHeight();
}
void Window::SetVSync(bool enabled)
{
    m_impl->SetVSync(enabled);
}
bool Window::IsVSync() const
{
    return m_impl->IsVSync();
}
void *Window::GetNativeWindow() const
{
    return m_impl->GetNativeWindow();
}

} // namespace Doodle

#endif
