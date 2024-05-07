#include "IndexBuffer.h"
#include "Renderer.h"
#include <Glad/glad.h>

namespace Doodle
{

class OpenGLIndexBuffer : public IndexBuffer
{
public:
    explicit OpenGLIndexBuffer(unsigned int size = 0) : m_rendererId(0), m_size(size)
    {
        std::function func = [this]() { glGenBuffers(1, &m_rendererId); };
        Renderer::Get().Submit(func);
    }

    ~OpenGLIndexBuffer()
    {
        std::function func = [this]() { glDeleteBuffers(1, &m_rendererId); };
        Renderer::Get().Submit(func);
    }

    void SetData(void *buffer, unsigned int size, unsigned int offset) override
    {
        std::function func = [this, buffer, size, offset]() {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererId);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, buffer, GL_STATIC_DRAW);
        };
        Renderer::Get().Submit(func);
    }

    void Bind() const override
    {
        std::function func = [this]() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererId); };
        Renderer::Get().Submit(func);
    }

    unsigned int GetSize() const override
    {
        return m_size;
    }
    RendererID GetRendererID() const override
    {
        return m_rendererId;
    }

private:
    unsigned int m_rendererId; // OpenGL buffer ID
    unsigned int m_size;       // Size of the buffer
};

std::unique_ptr<IndexBuffer> IndexBuffer::Create(unsigned int size)
{
    return std::make_unique<OpenGLIndexBuffer>(size);
}

} // namespace Doodle
