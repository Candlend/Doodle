#pragma once

#include "pch.h"

#include "Renderer.h"

namespace Doodle
{

struct BufferElement
{
    std::string Name;
    uint32_t Size;
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
    static std::shared_ptr<VertexBuffer> Create(uint32_t size = 0);
    ~VertexBuffer() = default;
    void SetData(void *buffer, uint32_t size)
    {
        SetData(buffer, size, 0);
    }
    virtual void SetData(void *buffer, uint32_t size, uint32_t offset) = 0;
    virtual void Bind() const = 0;
    // BufferLayout methods
    template <typename T> void PushElement(const std::string &name, bool normalized = false)
    {
        static_assert(std::is_trivial<T>::value, "Type must be trivial");
        Renderer::Submit([this, name, normalized]() {
            m_elements.emplace_back(sizeof(T), name, normalized);
            m_stride += sizeof(T);
            DOO_CORE_TRACE("VBO <{0}> pushed element: {1}, size={2}, stride={3}", m_rendererId, name, sizeof(T),
                           m_stride);
        });
    }

    inline const std::vector<BufferElement> &GetElements() const
    {
        return m_elements;
    }

    inline uint32_t GetRendererID() const
    {
        return m_rendererId;
    }

    inline uint32_t GetSize() const
    {
        return m_size;
    }

    inline uint32_t GetStride() const
    {
        return m_stride;
    }

protected:
    uint32_t m_rendererId;
    uint32_t m_size;
    std::vector<BufferElement> m_elements;
    uint32_t m_stride = 0;
};
using VBO = VertexBuffer;
} // namespace Doodle
