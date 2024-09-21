
#include <cstddef>
#include <cstdint>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

#include "Core.h"
#include "Log.h"
#include "RenderScope.h"
#include "Renderer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

namespace Doodle
{

std::pair<GLenum, size_t> GetGLType(VertexDataType type)
{
    static const std::unordered_map<VertexDataType, std::pair<GLenum, size_t>> TYPE_MAP = {
        {VertexDataType::Int, {GL_INT, sizeof(int)}},
        {VertexDataType::UnsignedInt, {GL_UNSIGNED_INT, sizeof(uint32_t)}},
        {VertexDataType::Short, {GL_SHORT, sizeof(int16_t)}},
        {VertexDataType::UnsignedShort, {GL_UNSIGNED_SHORT, sizeof(uint16_t)}},
        {VertexDataType::Char, {GL_BYTE, sizeof(char)}},
        {VertexDataType::UnsignedChar, {GL_UNSIGNED_BYTE, sizeof(unsigned char)}},
        {VertexDataType::Float, {GL_FLOAT, sizeof(float)}},
        {VertexDataType::Double, {GL_DOUBLE, sizeof(double)}},
        {VertexDataType::Bool, {GL_BOOL, sizeof(bool)}},
        {VertexDataType::Vec2, {GL_FLOAT, sizeof(float)}},
        {VertexDataType::Vec3, {GL_FLOAT, sizeof(float)}},
        {VertexDataType::Vec4, {GL_FLOAT, sizeof(float)}},
        {VertexDataType::Mat3, {GL_FLOAT, sizeof(float)}},
        {VertexDataType::Mat4, {GL_FLOAT, sizeof(float)}},
    };
    auto it = TYPE_MAP.find(type);
    DOO_CORE_ASSERT(it != TYPE_MAP.end(), "Unknown vertex data type");
    return it->second;
}

class OpenGLVertexArray : public VertexArray
{
public:
    OpenGLVertexArray()
    {
        Renderer::Submit([this]() { glGenVertexArrays(1, &m_rendererId); });
    }

    ~OpenGLVertexArray()
    {
        Renderer::Submit([this]() { glDeleteVertexArrays(1, &m_rendererId); });
    }

    uint32_t GetRendererID() const override
    {
        return m_rendererId;
    }

    void Bind() const override
    {
        Renderer::Submit([this]() { glBindVertexArray(m_rendererId); });
    }

    void Unbind() const override
    {
        Renderer::Submit([this]() { glBindVertexArray(0); });
    }

    void AddVertexBuffer(const std::shared_ptr<VertexBuffer> &vertexBuffer) override
    {
        Bind();
        vertexBuffer->Bind();
        m_vertexBuffers.push_back(vertexBuffer);

        Renderer::Submit([this, vertexBuffer]() {
            const auto &elements = vertexBuffer->GetElements();
            intptr_t offset = 0;
            for (size_t i = 0; i < elements.size(); i++)
            {
                const auto &element = elements[i];
                auto [glType, glSize] = GetGLType(element.Type);
                glEnableVertexAttribArray(i);
                glVertexAttribPointer(i,
                                      element.Size / glSize, // 计算属性的数量
                                      glType, element.Normalized ? GL_TRUE : GL_FALSE, vertexBuffer->GetStride(),
                                      reinterpret_cast<void *>(offset));
                offset += element.Size; // 更新偏移量
            }
        });
    }

    void SetIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer) override
    {
        Bind();
        indexBuffer->Bind();
        m_indexBuffer = indexBuffer;
    }

    const std::vector<std::shared_ptr<VertexBuffer>> &GetVertexBuffers() const override
    {
        return m_vertexBuffers;
    }

    const std::shared_ptr<IndexBuffer> &GetIndexBuffer() const override
    {
        return m_indexBuffer;
    }

    void Render() const override
    {
        Bind();
        if (m_indexBuffer)
            Renderer::DrawIndexed(m_indexBuffer->GetSize());
        else
            Renderer::Submit([this]() {
                auto count = m_vertexBuffers[0]->GetSize() / m_vertexBuffers[0]->GetStride();
                RendererAPI::Draw(count, PrimitiveType::Triangles);
            });
    }

private:
    uint32_t m_rendererId;
    uint32_t m_vertexBufferIndex = 0;
    std::vector<std::shared_ptr<VertexBuffer>> m_vertexBuffers;
    std::shared_ptr<IndexBuffer> m_indexBuffer;
};

std::shared_ptr<VertexArray> VertexArray::Create()
{
    return std::make_shared<OpenGLVertexArray>();
}

} // namespace Doodle