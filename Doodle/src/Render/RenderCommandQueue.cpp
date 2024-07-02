#include "pch.h"
#include "RenderCommandQueue.h"
#include "Log.h"

namespace Doodle
{

RenderCommandQueue::RenderCommandQueue() : m_currentBufferPtr(m_commandBuffer)
{
    DOO_CORE_TRACE("RenderCommandQueue::RenderCommandQueue");
}

RenderCommandQueue::~RenderCommandQueue()
{
    DOO_CORE_TRACE("RenderCommandQueue::~RenderCommandQueue");
}

void *RenderCommandQueue::Allocate(RenderCommandFn func, std::size_t size)
{
    // 检查是否会超出缓冲区
    if (m_commandCount >= BUFFER_SIZE || 
        m_currentBufferPtr + sizeof(RenderCommandFn) + sizeof(std::size_t) + size > m_commandBuffer + BUFFER_SIZE)
    {
        throw std::runtime_error("Command buffer overflow");
    }

    // 存储函数指针
    RenderCommandFn* funcPtr = reinterpret_cast<RenderCommandFn*>(m_currentBufferPtr);
    new (funcPtr) RenderCommandFn(std::move(func));
    m_currentBufferPtr += sizeof(RenderCommandFn);

    // 存储命令大小
    std::size_t* sizePtr = reinterpret_cast<std::size_t*>(m_currentBufferPtr);
    new (sizePtr) std::size_t(std::move(size));
    m_currentBufferPtr += sizeof(std::size_t);

    // 返回命令的内存位置
    void *memoryPtr = m_currentBufferPtr;
    m_currentBufferPtr += size;

    m_commandCount++;
    return memoryPtr;
}

void RenderCommandQueue::Execute()
{
    DOO_CORE_DEBUG("RenderCommandQueue::Execute {0} commands", m_commandCount);
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
    m_currentBufferPtr = m_commandBuffer;
    m_commandCount = 0;
}

} // namespace Doodle
