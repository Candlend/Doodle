#pragma once

#include "pch.h"

namespace Doodle
{

class DOO_API IndexBuffer
{
public:
    static std::shared_ptr<IndexBuffer> Create(uint32_t size = 0);
    ~IndexBuffer() = default;
    void SetData(void *buffer, uint32_t size)
    {
        SetData(buffer, size, 0);
    }
    virtual void SetData(void *buffer, uint32_t size, uint32_t offset) = 0;
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    inline uint32_t GetCount() const
    {
        return GetSize() / sizeof(uint32_t);
    }
    inline uint32_t GetSize() const
    {
        return m_size;
    }
    inline uint32_t GetRendererID() const
    {
        return m_rendererId;
    }

protected:
    uint32_t m_rendererId;
    uint32_t m_size;
};
using IBO = IndexBuffer;
} // namespace Doodle
