#pragma once

#include "pch.h"
#include <glm/glm.hpp>

#include "IndexBuffer.h"
#include "VertexBuffer.h"

namespace Doodle
{

class DOO_API Mesh
{
public:
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec3 Tangent;
        glm::vec3 Binormal;
        glm::vec2 Texcoord;
    };
    static_assert(sizeof(Vertex) == 14 * sizeof(float));
    static const int NUM_ATTRIBUTES = 5;

    struct Index
    {
        uint32_t V1, V2, V3;
    };
    static_assert(sizeof(Index) == 3 * sizeof(uint32_t));

    explicit Mesh(const std::string &filename);
    ~Mesh();

    void Render();

    inline const std::string &GetFilePath() const
    {
        return m_filePath;
    }

private:
    std::vector<Vertex> m_vertices;
    std::vector<Index> m_indices;

    std::unique_ptr<VertexBuffer> m_vertexBuffer;
    std::unique_ptr<IndexBuffer> m_indexBuffer;

    std::string m_filePath;
};
} // namespace Doodle
