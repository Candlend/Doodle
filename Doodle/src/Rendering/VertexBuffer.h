#pragma once

#include "pch.h"
#include <cstddef>
#include <typeinfo>
#include <vcruntime_typeinfo.h>

#include "Renderer.h"

namespace Doodle
{

enum class VertexDataType
{
    Int,
    UnsignedInt,
    Short,
    UnsignedShort,
    Char,
    UnsignedChar,
    Float,
    Double,
    Bool,
    Vec2,
    Vec3,
    Vec4,
    Mat3,
    Mat4
};

struct BufferElement
{
    std::string Name;
    VertexDataType Type;
    size_t Size;
    size_t Offset;
    bool Normalized;
};

class DOO_API VertexBuffer
{
public:
    static std::shared_ptr<VertexBuffer> Create(void *data, size_t size, bool dynamic = false);
    ~VertexBuffer() = default;
    void SetSubData(void *buffer, size_t size)
    {
        SetSubData(buffer, size, 0);
    }
    virtual void SetSubData(void *buffer, size_t size, size_t offset) = 0;
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    virtual void PushElement(const std::string &name, VertexDataType type, bool normalized) = 0;
    void PushElement(const std::string &name, VertexDataType type)
    {
        PushElement(name, type, false);
    }

    inline const std::vector<BufferElement> &GetElements() const
    {
        return m_elements;
    }

    inline size_t GetRendererID() const
    {
        return m_rendererId;
    }

    inline size_t GetSize() const
    {
        return m_size;
    }

    inline size_t GetStride() const
    {
        return m_stride;
    }

    inline bool IsDynamic() const
    {
        return m_dynamic;
    }

protected:
    uint32_t m_rendererId;
    size_t m_size;
    std::vector<BufferElement> m_elements;
    size_t m_stride = 0;
    bool m_dynamic;
};
using VBO = VertexBuffer;
} // namespace Doodle
