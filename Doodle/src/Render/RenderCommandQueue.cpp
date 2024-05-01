#include "RenderCommandQueue.h"
#include "pch.h"
#include <cstring>

namespace Doodle
{

RenderCommandQueue::RenderCommandQueue() : m_currentBufferPosition(m_commandBuffer)
{
}

RenderCommandQueue::~RenderCommandQueue()
{
}

void *RenderCommandQueue::Allocate(RenderCommandFn func, std::size_t size)
{
    if (m_commandCount >= BUFFER_SIZE)
    {
        throw std::runtime_error("Command buffer overflow");
    }

    // 存储函数指针
    *reinterpret_cast<RenderCommandFn *>(m_currentBufferPosition) = func;
    m_currentBufferPosition += sizeof(RenderCommandFn);

    // 存储命令大小
    *reinterpret_cast<std::size_t *>(m_currentBufferPosition) = size;
    m_currentBufferPosition += sizeof(std::size_t);

    // 返回命令的内存位置
    void *memory = m_currentBufferPosition;
    m_currentBufferPosition += size;

    m_commandCount++;
    return memory;
}

void RenderCommandQueue::Execute()
{
    std::byte *buffer = m_commandBuffer;

    for (unsigned int i = 0; i < m_commandCount; i++)
    {
        RenderCommandFn function = *reinterpret_cast<RenderCommandFn *>(buffer);
        buffer += sizeof(RenderCommandFn);

        std::size_t size = *reinterpret_cast<std::size_t *>(buffer);
        buffer += sizeof(std::size_t);
        function(buffer);
        buffer += size;
    }

    Reset();
}

void RenderCommandQueue::Reset()
{
    m_currentBufferPosition = m_commandBuffer;
    m_commandCount = 0;
}

} // namespace Doodle
