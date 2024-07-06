#pragma once

#include "pch.h"

namespace Doodle
{

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
    
    virtual uint32_t GetSize() const = 0;
    virtual uint32_t GetRendererID() const = 0;
};
using VBO = VertexBuffer;
} // namespace Doodle
