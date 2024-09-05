#include "pch.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "GraphicsContext.h"

namespace Doodle
{

class OpenGLContext : public GraphicsContext
{
public:
    OpenGLContext(GLFWwindow *window) : m_window(window)

    {
    }
    void Initialize() override
    {
        glfwMakeContextCurrent(m_window);
        int status = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
        DOO_CORE_ASSERT(status, "Failed to initialize Glad!");
    }

    void SwapBuffers() override
    {
        glfwSwapBuffers(m_window);
    }

private:
    GLFWwindow *m_window;
};

GraphicsContext *GraphicsContext::Create(void *window)
{
    return new OpenGLContext(static_cast<GLFWwindow *>(window));
}

} // namespace Doodle