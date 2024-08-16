
#include <cstddef>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "RenderScope.h"
#include "Renderer.h"
#include "VertexArray.h"

namespace Doodle
{

std::pair<GLenum, size_t> GetGLType(std::string typeName)
{
    // 创建一个映射表
    static const std::unordered_map<std::string, std::pair<GLenum, size_t>> TYPE_MAP = {
        {"int", {GL_INT, sizeof(int)}},           {"unsigned int", {GL_UNSIGNED_INT, sizeof(uint32_t)}},
        {"short", {GL_SHORT, sizeof(int16_t)}},   {"unsigned short", {GL_UNSIGNED_SHORT, sizeof(uint16_t)}},
        {"char", {GL_BYTE, sizeof(char)}},        {"unsigned char", {GL_UNSIGNED_BYTE, sizeof(unsigned char)}},
        {"float", {GL_FLOAT, sizeof(float)}},     {"double", {GL_DOUBLE, sizeof(double)}},
        {"bool", {GL_BOOL, sizeof(bool)}},        {"glm::vec2", {GL_FLOAT, sizeof(float)}},
        {"glm::vec3", {GL_FLOAT, sizeof(float)}}, {"glm::vec4", {GL_FLOAT, sizeof(float)}},
        {"glm::mat3", {GL_FLOAT, sizeof(float)}}, {"glm::mat4", {GL_FLOAT, sizeof(float)}},
    };
    auto it = TYPE_MAP.find(typeName);
    if (it != TYPE_MAP.end())
    {
        return it->second;
    }
    throw std::runtime_error("Unsupported type for OpenGL");
}

class OpenGLVertexArray : public VertexArray
{
public:
    OpenGLVertexArray()
    {
        Renderer::Submit([this]() {
            glGenVertexArrays(1, &m_rendererId);
            DOO_CORE_TRACE("VAO <{0}> created", m_rendererId);
        });
    }

    ~OpenGLVertexArray()
    {
        Renderer::Submit([this]() {
            glDeleteVertexArrays(1, &m_rendererId);
            DOO_CORE_TRACE("VAO <{0}> destroyed", m_rendererId);
        });
    }

    void Bind() const override
    {
        Renderer::Submit([this]() {
            glBindVertexArray(m_rendererId);
            DOO_CORE_TRACE("VAO <{0}> bound", m_rendererId);
        });
    }

    void Unbind() const override
    {
        Renderer::Submit([this]() {
            glBindVertexArray(0);
            DOO_CORE_TRACE("VAO <{0}> unbound", m_rendererId);
        });
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
                DOO_CORE_TRACE("VAO <{0}> added vertex buffer <{1}>", m_rendererId, vertexBuffer->GetRendererID());
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

    void Render(bool depthTest) const override
    {
        Bind();
        Renderer::DrawIndexed(m_indexBuffer->GetSize(), depthTest);
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