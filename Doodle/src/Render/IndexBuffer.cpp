#include "IndexBuffer.h"
#include "Log.h"
#include "Renderer.h"
#include <glad/glad.h>

namespace Doodle
{

class OpenGLIndexBuffer : public IndexBuffer
{
public:
    explicit OpenGLIndexBuffer(uint32_t size = 0)
    {
        m_size = size;
        Renderer::Submit([this]() {
            glGenBuffers(1, &m_rendererId);
            DOO_CORE_TRACE("IBO <{0}> created", m_rendererId);
        });
    }

    ~OpenGLIndexBuffer()
    {
        Renderer::Submit([this]() {
            glDeleteBuffers(1, &m_rendererId);
            DOO_CORE_TRACE("IBO <{0}> destroyed", m_rendererId);
        });
    }

    void SetData(void *buffer, uint32_t size, uint32_t offset) override
    {
        m_size = size;
        Renderer::Submit([this, buffer, size, offset]() {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererId);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, buffer, GL_STATIC_DRAW);
            DOO_CORE_TRACE("IBO <{0}> updated: size={1}, offset={2}", m_rendererId, size, offset);
        });
    }

    void Bind() const override
    {
        Renderer::Submit([this]() {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererId);
            DOO_CORE_TRACE("IBO <{0}> bound", m_rendererId);
        });
    }

    void Unbind() const override
    {
        Renderer::Submit([this]() {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            DOO_CORE_TRACE("IBO <{0}> unbound", m_rendererId);
        });
    }
};

std::shared_ptr<IndexBuffer> IndexBuffer::Create(uint32_t size)
{
    return std::make_shared<OpenGLIndexBuffer>(size);
}

} // namespace Doodle
