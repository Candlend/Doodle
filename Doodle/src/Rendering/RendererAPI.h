#pragma once

#include "pch.h"

namespace Doodle
{

enum class BufferFlags
{
    None = 0,
    Color = 1 << 0,
    Depth = 1 << 1,
    Stencil = 1 << 2,
    All = Color | Depth | Stencil
};

// 重载按位或运算符
inline BufferFlags operator|(BufferFlags lhs, BufferFlags rhs)
{
    return static_cast<BufferFlags>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

// 重载按位与运算符
inline BufferFlags operator&(BufferFlags lhs, BufferFlags rhs)
{
    return static_cast<BufferFlags>(static_cast<int>(lhs) & static_cast<int>(rhs));
}

// 重载输出运算符
inline std::ostream &operator<<(std::ostream &os, BufferFlags type)
{
    switch (type)
    {
    case BufferFlags::None:
        os << "None";
        break;
    case BufferFlags::Color:
        os << "Color";
        break;
    case BufferFlags::Depth:
        os << "Depth";
        break;
    case BufferFlags::Stencil:
        os << "Stencil";
        break;
    case BufferFlags::All:
        os << "All";
        break;
    default:
        os << "Unknown";
        break;
    }
    return os;
}

enum class DepthTestType
{
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always,
    Disabled
};

enum class CullFaceType
{
    Front,
    Back,
    FrontAndBack,
    Disabled
};

enum class BlendType
{
    Zero,
    One,
    SrcColor,
    OneMinusSrcColor,
    DstColor,
    OneMinusDstColor,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstAlpha,
    OneMinusDstAlpha,
    ConstantColor,
    OneMinusConstantColor,
    ConstantAlpha,
    OneMinusConstantAlpha,
    SrcAlphaSaturate
};

enum class BlendEquationType
{
    Add,
    Subtract,
    ReverseSubtract,
    Min,
    Max
};

enum class PolygonModeType
{
    Fill,
    Line,
    Point
};

enum class FrontFaceType
{
    Clockwise,
    CounterClockwise
};

enum class PrimitiveType
{
    Triangles,
    Lines,
    Points
};

struct RenderAPICapabilities
{
    std::string Vendor;
    std::string Renderer;
    std::string Version;

    int MaxSamples;
    float MaxAnisotropy;
};

class DOO_API RendererAPI
{
public:
    static void Initialize();

    static void Clear(BufferFlags bufferFlags = BufferFlags::All);
    static void SetClearColor(float r, float g, float b, float a);

    static void DrawIndexed(unsigned int count);
    static void Draw(unsigned int count, PrimitiveType type);

    static void SetDepthWrite(bool write);
    static void SetDepthTest(DepthTestType type);
    static void SetCullFace(CullFaceType type);
    static void SetBlend(BlendType src, BlendType dst);
    static void SetBlendEquation(BlendEquationType type);
    static void SetPolygonMode(PolygonModeType type);
    static void SetFrontFace(FrontFaceType type);
    static void SetPrimitiveType(PrimitiveType type);

    static RenderAPICapabilities &GetCapabilities()
    {
        static RenderAPICapabilities s_Capabilities;
        return s_Capabilities;
    }
};

} // namespace Doodle