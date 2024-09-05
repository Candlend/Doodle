#pragma once

#include "pch.h"

#include "IndexBuffer.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include <cstdint>

namespace Doodle
{

class DOO_API VertexArray
{
public:
    static std::shared_ptr<VertexArray> Create();
    ~VertexArray() = default;
    virtual uint32_t GetRendererID() const = 0;
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    virtual void Render(bool depthTest) const = 0;
    void Render() const
    {
        Render(true);
    }

    virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer> &vertexBuffer) = 0;
    virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer) = 0;

    virtual const std::vector<std::shared_ptr<VertexBuffer>> &GetVertexBuffers() const = 0;
    virtual const std::shared_ptr<IndexBuffer> &GetIndexBuffer() const = 0;
};
using VAO = VertexArray;
} // namespace Doodle
