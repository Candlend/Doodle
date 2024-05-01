#pragma once

#include "pch.h"

#include "RenderCommand.h"

namespace Doodle
{

class DOO_API RenderCommandQueue
{
public:
    using RenderCommandFn = std::function<void(void *)>;

    RenderCommandQueue();
    ~RenderCommandQueue();

    void *Allocate(RenderCommandFn func, std::size_t size);
    void Execute();

private:
    static constexpr std::size_t BUFFER_SIZE = 10 * 1024 * 1024;
    std::byte m_commandBuffer[BUFFER_SIZE] = {};
    std::byte *m_currentBufferPosition;
    unsigned int m_commandCount = 0;

    void Reset();
};

} // namespace Doodle
