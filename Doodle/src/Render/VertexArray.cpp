#include "Log.h"
#include "RenderScope.h"
#include "Renderer.h"
#include "glad/glad.h"

#include "VertexArray.h"

namespace Doodle
{

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
                glEnableVertexAttribArray(i);
                glVertexAttribPointer(i,
                                      element.Size / sizeof(float), // 计算属性的数量
                                      GL_FLOAT, element.Normalized ? GL_TRUE : GL_FALSE, vertexBuffer->GetStride(),
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