#pragma once

#include "pch.h"

namespace Doodle
{
enum class TextureFormat
{
    None = 0,
    RGB8,
    RGB16F,
    RGB32F,
    RGBA8,
    RGBA16F,
    RGBA32F,

    SRGB8,
    SRGB8ALPHA8,

    DEPTH32F,
    DEPTH24STENCIL8,

    // Defaults
    Depth = DEPTH24STENCIL8
};

std::string TextureFormatToString(TextureFormat format)
{
    switch (format)
    {
    case TextureFormat::RGB8:
        return "RGB8";
    case TextureFormat::RGB16F:
        return "RGB16F";
    case TextureFormat::RGB32F:
        return "RGB32F";
    case TextureFormat::RGBA8:
        return "RGBA8";
    case TextureFormat::RGBA16F:
        return "RGBA16F";
    case TextureFormat::RGBA32F:
        return "RGBA32F";
    case TextureFormat::SRGB8:
        return "SRGB8";
    case TextureFormat::SRGB8ALPHA8:
        return "SRGB8ALPHA8";
    case TextureFormat::DEPTH32F:
        return "DEPTH32F";
    case TextureFormat::DEPTH24STENCIL8:
        return "DEPTH24STENCIL8";
    default:
        return "None";
    }
}

enum class TextureWrap
{
    None = 0,
    Repeat = 1,
    MirroredRepeat = 2,
    Clamp = 3,
};

std::string TextureWrapToString(TextureWrap wrap)
{
    switch (wrap)
    {
    case TextureWrap::Repeat:
        return "Repeat";
    case TextureWrap::MirroredRepeat:
        return "MirroredRepeat";
    case TextureWrap::Clamp:
        return "Clamp";
    default:
        return "None";
    }
}

enum class TextureFilter
{
    None = 0,
    Nearest = 1,
    Linear = 2,
    MipmapNearest = 3,
    MipmapLinear = 4,
};

std::string TextureFilterToString(TextureFilter filter)
{
    switch (filter)
    {
    case TextureFilter::Nearest:
        return "Nearest";
    case TextureFilter::Linear:
        return "Linear";
    case TextureFilter::MipmapNearest:
        return "MipmapNearest";
    case TextureFilter::MipmapLinear:
        return "MipmapLinear";
    default:
        return "None";
    }
}

struct TextureParams
{
    TextureFormat Format = TextureFormat::RGBA8;
    TextureWrap Wrap = TextureWrap::Repeat;
    TextureFilter Filter = TextureFilter::Linear;

    std::string ToString() const
    {
        std::ostringstream oss;
        oss << "Format: " << TextureFormatToString(Format) << ", Wrap: " << TextureWrapToString(Wrap)
            << ", Filter: " << TextureFilterToString(Filter);
        return oss.str();
    }
};

} // namespace Doodle