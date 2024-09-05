#include "TextureParams.h"

namespace Doodle
{

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

} // namespace Doodle