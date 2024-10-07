#pragma once

#include "pch.h"

#include "UUID.h"

namespace Doodle
{

enum class PrimitiveTextureType
{
    None,
    White,
    Black,
    Normal,
};

struct Texture2DInfo
{
    UUID UUID;
    PrimitiveTextureType Type = PrimitiveTextureType::None;

    Texture2DInfo(PrimitiveTextureType type = PrimitiveTextureType::None) : Type(type)
    {
    }

    Texture2DInfo(Doodle::UUID uuid, PrimitiveTextureType type = PrimitiveTextureType::None) : UUID(uuid), Type(type)
    {
    }
};

} // namespace Doodle

// NOLINTBEGIN
namespace rfl
{
template <> struct Reflector<Doodle::Texture2DInfo>
{
    struct ReflType
    {
        Doodle::UUID UUID;
        Doodle::PrimitiveTextureType Type;
    };

    static Doodle::Texture2DInfo to(const ReflType &v) noexcept
    {
        return {v.UUID, v.Type};
    }

    static ReflType from(const Doodle::Texture2DInfo &v)
    {
        return {v.UUID, v.Type};
    }
};
} // namespace rfl
// NOLINTEND