#pragma once

#include "pch.h"

#include "Renderer.h"

namespace Doodle
{


class DOO_API VertexBuffer
{
public:
    static std::unique_ptr<VertexBuffer> Create(unsigned int size = 0);
    ~VertexBuffer() = default;
    void SetData(void *buffer, unsigned int size) { SetData(buffer, size, 0); }
    virtual void SetData(void *buffer, unsigned int size, unsigned int offset) = 0;
    virtual void Bind() const = 0;

    virtual unsigned int GetSize() const = 0;
    virtual RendererID GetRendererID() const = 0;
};

} // namespace Doodle
