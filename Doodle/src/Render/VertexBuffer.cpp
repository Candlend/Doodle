#include "VertexBuffer.h"
#include "Log.h"
#include "Renderer.h"
#include <glad/glad.h>
#include <memory>

namespace Doodle
{

class OpenGLVertexBuffer : public VertexBuffer
{
public:
    explicit OpenGLVertexBuffer(uint32_t size = 0) : m_rendererId(0), m_size(size)
    {
        Renderer::Submit([this]() {
            glGenBuffers(1, &m_rendererId);
            DOO_CORE_TRACE("VBO <{0}> created", m_rendererId);
        });
    }

    ~OpenGLVertexBuffer()
    {
        Renderer::Submit([this]() {
            glDeleteBuffers(1, &m_rendererId);
            DOO_CORE_TRACE("VBO <{0}> destroyed", m_rendererId);
        });
    }

    void SetData(void *buffer, uint32_t size, uint32_t offset) override
    {
        Renderer::Submit([this, buffer, size, offset]() {
            glBindBuffer(GL_ARRAY_BUFFER, m_rendererId);
            glBufferData(GL_ARRAY_BUFFER, size, buffer, GL_STATIC_DRAW);

            DOO_CORE_TRACE("VBO <{0}> updated: size={1}, offset={2}", m_rendererId, size, offset);
        });
    }

    void Bind() const override
    {
        Renderer::Submit([this]() {
            glBindBuffer(GL_ARRAY_BUFFER, m_rendererId);
            DOO_CORE_TRACE("VBO <{0}> bound", m_rendererId);
        });
    }

    uint32_t GetSize() const override
    {
        return m_size;
    }
    uint32_t GetRendererID() const override
    {
        return m_rendererId;
    }

private:
    uint32_t m_rendererId; // OpenGL buffer ID
    uint32_t m_size;       // Size of the buffer
};

std::shared_ptr<VertexBuffer> VertexBuffer::Create(uint32_t size)
{
    return std::make_shared<OpenGLVertexBuffer>(size);
}

} // namespace Doodle
