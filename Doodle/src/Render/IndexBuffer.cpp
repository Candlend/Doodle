#include "IndexBuffer.h"
#include "Renderer.h"
#include <glad/glad.h>

namespace Doodle
{

class OpenGLIndexBuffer : public IndexBuffer
{
public:
    explicit OpenGLIndexBuffer(uint32_t size = 0) : m_rendererId(0), m_size(size)
    {
        Renderer::Submit([this]() { glGenBuffers(1, &m_rendererId); });
    }

    ~OpenGLIndexBuffer()
    {
        Renderer::Submit([this]() { glDeleteBuffers(1, &m_rendererId); });
    }

    void SetData(void *buffer, uint32_t size, uint32_t offset) override
    {
        m_size = size;
        Renderer::Submit([this, buffer, size, offset]() {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererId);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, buffer, GL_STATIC_DRAW);
        });
    }

    void Bind() const override
    {
        Renderer::Submit([this]() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererId); });
    }

    uint32_t GetCount() const override
    {
        return m_size / sizeof(uint32_t);
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

std::shared_ptr<IndexBuffer> IndexBuffer::Create(uint32_t size)
{
    return std::make_shared<OpenGLIndexBuffer>(size);
}

} // namespace Doodle
