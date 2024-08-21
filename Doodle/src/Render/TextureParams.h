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

enum class TextureWrap
{
    None = 0,
    Repeat = 1,
    MirroredRepeat = 2,
    Clamp = 3,
};

enum class TextureFilter
{
    None = 0,
    Nearest = 1,
    Linear = 2,
    MipmapNearest = 3,
    MipmapLinear = 4,
};
DOO_API std::string TextureFormatToString(TextureFormat format);
DOO_API std::string TextureWrapToString(TextureWrap wrap);
DOO_API std::string TextureFilterToString(TextureFilter filter);

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