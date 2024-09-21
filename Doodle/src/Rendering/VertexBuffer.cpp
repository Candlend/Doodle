#include "VertexBuffer.h"
#include "Log.h"
#include "Renderer.h"
#include <cstddef>
#include <glad/glad.h>
#include <memory>

namespace Doodle
{

size_t GetSize(VertexDataType type)
{
    switch (type)
    {
    case VertexDataType::Int:
        return sizeof(int);
    case VertexDataType::UnsignedInt:
        return sizeof(uint32_t);
    case VertexDataType::Short:
        return sizeof(int16_t);
    case VertexDataType::UnsignedShort:
        return sizeof(uint16_t);
    case VertexDataType::Char:
        return sizeof(char);
    case VertexDataType::UnsignedChar:
        return sizeof(unsigned char);
    case VertexDataType::Float:
        return sizeof(float);
    case VertexDataType::Double:
        return sizeof(double);
    case VertexDataType::Bool:
        return sizeof(bool);
    case VertexDataType::Vec2:
        return sizeof(float) * 2;
    case VertexDataType::Vec3:
        return sizeof(float) * 3;
    case VertexDataType::Vec4:
        return sizeof(float) * 4;
    case VertexDataType::Mat3:
        return sizeof(float) * 9;
    case VertexDataType::Mat4:
        return sizeof(float) * 16;
    default:
        DOO_CORE_ASSERT(false, "Unknown vertex data type");
        return 0;
    }
}

class OpenGLVertexBuffer : public VertexBuffer
{
public:
    OpenGLVertexBuffer(void *data, size_t size, bool dynamic)
    {
        m_dynamic = dynamic;
        m_size = size;
        Renderer::Submit([this, data]() {
            glCreateBuffers(1, &m_rendererId);
            glNamedBufferData(m_rendererId, m_size, data, m_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        });
    }

    ~OpenGLVertexBuffer()
    {
        Renderer::Submit([this]() { glDeleteBuffers(1, &m_rendererId); });
    }

    void SetSubData(void *buffer, size_t size, size_t offset) override
    {
        if (offset + size > m_size)
        {
            DOO_CORE_ERROR("VBO <{0}> size exceeded", m_rendererId);
            return;
        }
        Renderer::Submit([this, buffer, size, offset]() { glNamedBufferSubData(m_rendererId, offset, size, buffer); });
    }

    void Bind() const override
    {
        Renderer::Submit([this]() { glBindBuffer(GL_ARRAY_BUFFER, m_rendererId); });
    }

    void Unbind() const override
    {
        Renderer::Submit([this]() { glBindBuffer(GL_ARRAY_BUFFER, 0); });
    }

    void PushElement(const std::string &name, VertexDataType type, bool normalized) override
    {
        Renderer::Submit([this, name, normalized, type]() {
            BufferElement element{name, type, ::Doodle::GetSize(type), m_stride, normalized};
            m_elements.emplace_back(element);
            m_stride += element.Size;
        });
    }
};

std::shared_ptr<VertexBuffer> VertexBuffer::Create(void *data, size_t size, bool dynamic)
{
    return std::make_shared<OpenGLVertexBuffer>(data, size, dynamic);
}

} // namespace Doodle
