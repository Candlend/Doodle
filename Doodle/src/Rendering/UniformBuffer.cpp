#include <cstdint>
#include <glad/glad.h>
#include <sstream>

#include "Log.h"
#include "Renderer.h"
#include "UniformBuffer.h"

namespace Doodle
{

class OpenGLUniformBuffer : public UniformBuffer
{
public:
    OpenGLUniformBuffer(void *data, size_t size, bool dynamic)
    {
        m_size = size;
        m_dynamic = dynamic;
        Renderer::Submit([this, data]() {
            glCreateBuffers(1, &m_rendererId);
            glNamedBufferData(m_rendererId, m_size, data, m_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
            DOO_CORE_DEBUG("UBO <{0}> created: size={1}, dynamic={2}", m_rendererId, m_size, m_dynamic);
        });
    }

    OpenGLUniformBuffer(size_t size, bool dynamic)
    {
        m_size = size;
        m_dynamic = dynamic;
        Renderer::Submit([this]() {
            glCreateBuffers(1, &m_rendererId);
            glNamedBufferData(m_rendererId, m_size, nullptr, m_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
            DOO_CORE_DEBUG("UBO <{0}> created: size={1}, dynamic={2}", m_rendererId, m_size, m_dynamic);
        });
    }

    ~OpenGLUniformBuffer()
    {
        Renderer::Submit([this]() { glDeleteBuffers(1, &m_rendererId); });
    }

    void SetSubData(const void *data, size_t size, size_t offset) override
    {
        if (offset + size > m_size)
        {
            DOO_CORE_ERROR("UBO <{0}> size exceeded", m_rendererId);
            return;
        }

        Renderer::Submit([this, data, size, offset]() { glNamedBufferSubData(m_rendererId, offset, size, data); });
    }

    void Bind(uint32_t slot) override
    {
        m_binding = slot;
        Renderer::Submit([this]() { glBindBufferBase(GL_UNIFORM_BUFFER, m_binding, m_rendererId); });
    }

    void Unbind() const override
    {
        Renderer::Submit([this]() { glBindBufferBase(GL_UNIFORM_BUFFER, m_binding, 0); });
    }

    uint32_t GetRendererID() const override
    {
        return m_rendererId;
    }

    uint32_t GetBinding() const override
    {
        return m_binding;
    }

    bool IsDynamic() const override
    {
        return m_dynamic;
    }

private:
    uint32_t m_rendererId;
    size_t m_size;
    uint32_t m_binding;
    bool m_dynamic;
};

std::shared_ptr<UniformBuffer> UniformBuffer::Create(void *data, size_t size, bool dynamic)
{
    return std::make_shared<OpenGLUniformBuffer>(data, size, dynamic);
}

std::shared_ptr<UniformBuffer> UniformBuffer::Create(size_t size, bool dynamic)
{
    return std::make_shared<OpenGLUniformBuffer>(size, dynamic);
}

} // namespace Doodle
