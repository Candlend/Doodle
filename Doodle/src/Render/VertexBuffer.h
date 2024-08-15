#pragma once

#include "pch.h"

#include "Renderer.h"
#include <cstdint>

namespace Doodle
{

struct BufferElement
{
    std::string Name;
    size_t Size;
    size_t Offset;
    bool Normalized;

    BufferElement(uint32_t size, const std::string &name, bool normalized = false)
        : Name(name), Size(size), Offset(0), Normalized(normalized)
    {
    }
};

class DOO_API VertexBuffer
{
public:
    static std::shared_ptr<VertexBuffer> Create(void *data, size_t size, bool dynamic = false);
    ~VertexBuffer() = default;
    void SetData(void *buffer, size_t size)
    {
        SetData(buffer, size, 0);
    }
    virtual void SetData(void *buffer, size_t size, size_t offset) = 0;
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    template <typename T> void PushElement(const std::string &name, bool normalized = false)
    {
        static_assert(std::is_trivial<T>::value, "Type must be trivial");
        Renderer::Submit([this, name, normalized]() {
            BufferElement element(sizeof(T), name, normalized);
            m_elements.emplace_back(element);
        });
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

protected:
    uint32_t m_rendererId;
    size_t m_size;
    std::vector<BufferElement> m_elements;
    size_t m_stride = 0;
    bool m_dynamic;
};
using VBO = VertexBuffer;
} // namespace Doodle
