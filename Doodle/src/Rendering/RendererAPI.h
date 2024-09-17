#pragma once

#include "pch.h"

namespace Doodle
{

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

    static void Clear(float r, float g, float b, float a);
    static void Clear();
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