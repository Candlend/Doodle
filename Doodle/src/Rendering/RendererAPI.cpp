
#include "pch.h"
#include <GLFW/glfw3.h>
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
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

void RendererAPI::Draw(unsigned int count, PrimitiveType type)
{
    GLenum glType;
    switch (type)
    {
    case PrimitiveType::Triangles:
        glType = GL_TRIANGLES;
        break;
    case PrimitiveType::Lines:
        glType = GL_LINES;
        break;
    case PrimitiveType::Points:
        glType = GL_POINTS;
        break;
    }
    glDrawArrays(glType, 0, count);
}

void RendererAPI::SetDepthTest(DepthTestType type)
{
    switch (type)
    {
    case DepthTestType::Less:
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        break;
    case DepthTestType::LessEqual:
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        break;
    case DepthTestType::Equal:
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_EQUAL);
        break;
    case DepthTestType::Greater:
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_GREATER);
        break;
    case DepthTestType::NotEqual:
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_NOTEQUAL);
        break;
    case DepthTestType::GreaterEqual:
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_GEQUAL);
        break;
    case DepthTestType::Always:
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_ALWAYS);
        break;
    case DepthTestType::Never:
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_NEVER);
        break;
    case DepthTestType::Disabled:
        glDisable(GL_DEPTH_TEST);
        break;
    }
}

void RendererAPI::SetDepthWrite(bool write)
{
    glDepthMask(write);
}

void RendererAPI::SetCullFace(CullFaceType type)
{
    switch (type)
    {
    case CullFaceType::Front:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        break;
    case CullFaceType::Back:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        break;
    case CullFaceType::FrontAndBack:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT_AND_BACK);
        break;
    }
}

void RendererAPI::SetBlendEquation(BlendEquationType type)
{
    switch (type)
    {
    case BlendEquationType::Add:
        glBlendEquation(GL_FUNC_ADD);
        break;
    case BlendEquationType::Subtract:
        glBlendEquation(GL_FUNC_SUBTRACT);
        break;
    case BlendEquationType::ReverseSubtract:
        glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
        break;
    case BlendEquationType::Min:
        glBlendEquation(GL_MIN);
        break;
    case BlendEquationType::Max:
        glBlendEquation(GL_MAX);
        break;
    }
}

void RendererAPI::SetPolygonMode(PolygonModeType type)
{
    switch (type)
    {
    case PolygonModeType::Fill:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    case PolygonModeType::Line:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case PolygonModeType::Point:
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        break;
    }
}

void RendererAPI::SetFrontFace(FrontFaceType type)
{
    switch (type)
    {
    case FrontFaceType::Clockwise:
        glFrontFace(GL_CW);
        break;
    case FrontFaceType::CounterClockwise:
        glFrontFace(GL_CCW);
        break;
    }
}

void RendererAPI::SetPrimitiveType(PrimitiveType type)
{
    switch (type)
    {
    case PrimitiveType::Triangles:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    case PrimitiveType::Lines:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case PrimitiveType::Points:
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        break;
    }
}

static GLenum GetGLBlendType(BlendType blendType)
{
    switch (blendType)
    {
    case BlendType::Zero:
        return GL_ZERO;
    case BlendType::One:
        return GL_ONE;
    case BlendType::SrcColor:
        return GL_SRC_COLOR;
    case BlendType::OneMinusSrcColor:
        return GL_ONE_MINUS_SRC_COLOR;
    case BlendType::DstColor:
        return GL_DST_COLOR;
    case BlendType::OneMinusDstColor:
        return GL_ONE_MINUS_DST_COLOR;
    case BlendType::SrcAlpha:
        return GL_SRC_ALPHA;
    case BlendType::OneMinusSrcAlpha:
        return GL_ONE_MINUS_SRC_ALPHA;
    case BlendType::DstAlpha:
        return GL_DST_ALPHA;
    case BlendType::OneMinusDstAlpha:
        return GL_ONE_MINUS_DST_ALPHA;
    case BlendType::ConstantColor:
        return GL_CONSTANT_COLOR;
    case BlendType::OneMinusConstantColor:
        return GL_ONE_MINUS_CONSTANT_COLOR;
    case BlendType::ConstantAlpha:
        return GL_CONSTANT_ALPHA;
    case BlendType::OneMinusConstantAlpha:
        return GL_ONE_MINUS_CONSTANT_ALPHA;
    case BlendType::SrcAlphaSaturate:
        return GL_SRC_ALPHA_SATURATE;
    default:
        return GL_ZERO;
    }
}

void RendererAPI::SetBlend(BlendType src, BlendType dst)
{
    glBlendFunc(GetGLBlendType(src), GetGLBlendType(dst));
}

} // namespace Doodle