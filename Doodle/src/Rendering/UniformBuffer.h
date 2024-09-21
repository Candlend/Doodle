#pragma once

#include "Log.h"
#include "pch.h"
#include <cstdint>
#include <glm/glm.hpp>

#include "Renderer.h"

namespace Doodle
{

class DOO_API UniformBuffer
{
public:
    static std::shared_ptr<UniformBuffer> Create(void *data, size_t size, bool dynamic = false);
    static std::shared_ptr<UniformBuffer> Create(size_t size, bool dynamic = false);

    virtual void SetSubData(const void *data, size_t size, size_t offset) = 0;
    void SetSubData(const void *data, size_t size)
    {
        SetSubData(data, size, 0);
    }
    virtual void Bind(uint32_t slot) = 0;
    void Bind()
    {
        Bind(0);
    }
    virtual void Unbind() const = 0;
    virtual uint32_t GetRendererID() const = 0;
    virtual uint32_t GetBinding() const = 0;
    virtual bool IsDynamic() const = 0;
};

using UBO = UniformBuffer;

} // namespace Doodle
