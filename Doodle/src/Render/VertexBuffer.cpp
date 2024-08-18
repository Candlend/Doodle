#include "VertexBuffer.h"
#include "Log.h"
#include "Renderer.h"
#include <cstddef>
#include <glad/glad.h>
#include <memory>

namespace Doodle
{

class OpenGLVertexBuffer : public VertexBuffer
{
public:
    explicit OpenGLVertexBuffer(void *data, size_t size, bool dynamic)
    {
        m_dynamic = dynamic;
        m_size = size;
        Renderer::Submit([this, data]() {
            glCreateBuffers(1, &m_rendererId);
            glNamedBufferData(m_rendererId, m_size, data, m_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
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

    void SetData(void *buffer, size_t size, size_t offset) override
    {
        if (offset + size > m_size)
        {
            DOO_CORE_ERROR("VBO <{0}> size exceeded", m_rendererId);
            return;
        }
        Renderer::Submit([this, buffer, size, offset]() {
            glNamedBufferSubData(m_rendererId, offset, size, buffer);
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

    void Unbind() const override
    {
        Renderer::Submit([this]() {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            DOO_CORE_TRACE("VBO <{0}> unbound", m_rendererId);
        });
    }
};

std::shared_ptr<VertexBuffer> VertexBuffer::Create(void *data, size_t size, bool dynamic)
{
    return std::make_shared<OpenGLVertexBuffer>(data, size, dynamic);
}

} // namespace Doodle
