#pragma once

#include "pch.h"

#include "UUID.h"

namespace Doodle
{

enum class PrimitiveMeshType
{
    None,
    Cube,
    Quad,
    Sphere,
    Plane,
};

struct MeshInfo
{
    UUID ModelUUID;
    unsigned int MeshIndex;
    std::string Name;
    PrimitiveMeshType Type = PrimitiveMeshType::None;

    MeshInfo(UUID modelUUID, unsigned int meshIndex, std::string name, PrimitiveMeshType type = PrimitiveMeshType::None)
        : ModelUUID(modelUUID), MeshIndex(meshIndex), Name(name), Type(type)
    {
    }

    MeshInfo(PrimitiveMeshType type) : Type(type)
    {
    }

    MeshInfo() = default;
};

} // namespace Doodle

// NOLINTBEGIN
namespace rfl
{
template <> struct Reflector<Doodle::MeshInfo>
{
    struct ReflType
    {
        Doodle::UUID ModelUUID;
        unsigned int MeshIndex;
        std::string Name;
        Doodle::PrimitiveMeshType Type;
    };

    static Doodle::MeshInfo to(const ReflType &v) noexcept
    {
        return {v.ModelUUID, v.MeshIndex, v.Name, v.Type};
    }

    static ReflType from(const Doodle::MeshInfo &v)
    {
        return {v.ModelUUID, v.MeshIndex, v.Name, v.Type};
    }
};
} // namespace rfl
// NOLINTEND