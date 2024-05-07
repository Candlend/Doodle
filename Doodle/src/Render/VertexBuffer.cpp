#include "VertexBuffer.h"
#include "Renderer.h"
#include <Glad/glad.h>

namespace Doodle
{

class OpenGLVertexBuffer : public VertexBuffer
{
public:
    explicit OpenGLVertexBuffer(unsigned int size = 0) : m_rendererId(0), m_size(size)
    {
        std::function func = [this]() { glGenBuffers(1, &m_rendererId); };
        Renderer::Get().Submit(func);
    }

    ~OpenGLVertexBuffer()
    {
        std::function func = [this]() { glDeleteBuffers(1, &m_rendererId); };
        Renderer::Get().Submit(func);
    }

    void SetData(void *buffer, unsigned int size, unsigned int offset) override
    {
        std::function func = [this, buffer, size, offset]() {
            glBindBuffer(GL_ARRAY_BUFFER, m_rendererId);
            glBufferData(GL_ARRAY_BUFFER, size, buffer, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        };
        Renderer::Get().Submit(func);
    }

    void Bind() const override
    {
        std::function func = [this]() { glBindBuffer(GL_ARRAY_BUFFER, m_rendererId); };
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

std::unique_ptr<VertexBuffer> VertexBuffer::Create(unsigned int size)
{
    return std::make_unique<OpenGLVertexBuffer>(size);
}

} // namespace Doodle
