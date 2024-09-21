#include "IndexBuffer.h"
#include "Log.h"
#include "Renderer.h"
#include <glad/glad.h>

namespace Doodle
{

class OpenGLIndexBuffer : public IndexBuffer
{
public:
    OpenGLIndexBuffer(void *data, size_t size)
    {
        m_size = size;
        Renderer::Submit([this, data]() {
            glCreateBuffers(1, &m_rendererId);
            glNamedBufferData(m_rendererId, m_size, data, GL_STATIC_DRAW);
        });
    }

    ~OpenGLIndexBuffer()
    {
        Renderer::Submit([this]() { glDeleteBuffers(1, &m_rendererId); });
    }

    void SetSubData(void *buffer, uint32_t size, uint32_t offset) override
    {
        if (offset + size > m_size)
        {
            return;
        }
        Renderer::Submit([this, buffer, size, offset]() { glNamedBufferSubData(m_rendererId, offset, size, buffer); });
    }

    void Bind() const override
    {
        Renderer::Submit([this]() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererId); });
    }

    void Unbind() const override
    {
        Renderer::Submit([this]() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); });
    }
};

std::shared_ptr<IndexBuffer> IndexBuffer::Create(void *data, size_t size)
{
    return std::make_shared<OpenGLIndexBuffer>(data, size);
}

} // namespace Doodle
