
#include "pch.h"
#include <glad/glad.h>

#include "RendererAPI.h"

namespace Doodle
{

static void OpenGLLogMessage(GLenum /*source*/, GLenum /*type*/, GLuint /*id*/, GLenum severity, GLsizei /*length*/,
                             const GLchar *message, const void * /*userParam*/)
{
    if (severity == GL_DEBUG_SEVERITY_LOW)
        DOO_CORE_INFO(message);
    else if (severity == GL_DEBUG_SEVERITY_MEDIUM)
        DOO_CORE_WARN(message);
    else if (severity == GL_DEBUG_SEVERITY_HIGH)
        DOO_CORE_ERROR(message);
}

void RendererAPI::Initialize()
{
    glDebugMessageCallback(OpenGLLogMessage, nullptr);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glFrontFace(GL_CCW);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto &caps = GetCapabilities();

    caps.Vendor = reinterpret_cast<const char *>(glGetString(GL_VENDOR));
    caps.Renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
    caps.Version = reinterpret_cast<const char *>(glGetString(GL_VERSION));

    glGetIntegerv(GL_MAX_SAMPLES, &caps.MaxSamples);
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &caps.MaxAnisotropy);

    GLenum error = glGetError();
    while (error != GL_NO_ERROR)
    {
        DOO_CORE_ERROR("OpenGL Error {0}", error);
        error = glGetError();
    }

    DOO_CORE_INFO("OpenGL Initialized with Vendor: {0}, Renderer: {1}, Version: {2}", caps.Vendor, caps.Renderer,
                  caps.Version);
}

void RendererAPI::Clear(float r, float g, float b, float a)
{
    SetClearColor(r, g, b, a);
    Clear();
}

void RendererAPI::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RendererAPI::SetClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

void RendererAPI::DrawIndexed(unsigned int count)
{
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}

void RendererAPI::UseWireframe(bool useWireframe)
{
    if (useWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

} // namespace Doodle