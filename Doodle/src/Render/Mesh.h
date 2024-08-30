#pragma once

#include "pch.h"
#include <glm/glm.hpp>

#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"


namespace Doodle
{

#pragma pack(push, 1)
struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec3 Tangent;
    glm::vec3 Binormal;
    glm::vec2 Texcoord;
};
#pragma pack(pop)

class DOO_API Mesh
{
public:
    static std::shared_ptr<Mesh> Create(const std::string &filename);
    Mesh(const std::string &filename);
    ~Mesh();

    void Render();

private:
    std::string m_filePath;
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;

    std::shared_ptr<VertexBuffer> m_vertexBuffer;
    std::shared_ptr<IndexBuffer> m_indexBuffer;
    std::shared_ptr<VertexArray> m_vertexArray; // 新增：VAO
};

} // namespace Doodle
