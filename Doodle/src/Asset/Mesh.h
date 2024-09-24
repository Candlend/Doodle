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
    glm::vec2 TexCoord;
};
#pragma pack(pop)

class Texture2D;
class DOO_API Mesh
{
public:
    static std::shared_ptr<Mesh> Create(const std::string &filename);
    Mesh(const std::string &filename);
    Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices,
         const std::unordered_map<std::string, std::shared_ptr<Texture2D>> &textures,
         const std::unordered_map<std::string, float> &uniform1f,
         const std::unordered_map<std::string, glm::vec4> &uniform4f);
    ~Mesh();

    const std::string &GetPath() const
    {
        return m_filepath;
    }
    void Render();

    static std::shared_ptr<Mesh> GetQuad();
    static std::shared_ptr<Mesh> GetCube();
    static std::shared_ptr<Mesh> GetSphere();
    static std::shared_ptr<Mesh> GetPlane();

    std::shared_ptr<Texture2D> GetTexture(const std::string &name);
    std::unordered_map<std::string, std::shared_ptr<Texture2D>> &GetTextures()
    {
        return m_textures;
    }
    std::unordered_map<std::string, float> &GetUniform1f()
    {
        return m_uniform1f;
    }
    std::unordered_map<std::string, glm::vec4> &GetUniform4f()
    {
        return m_uniform4f;
    }
    uint32_t GetVertexCount() const
    {
        return m_vertices.size();
    }
    uint32_t GetFaceCount() const
    {
        return m_indices.size() / 3;
    }

private:
    std::string m_filepath;
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_textures;
    std::unordered_map<std::string, float> m_uniform1f;
    std::unordered_map<std::string, glm::vec4> m_uniform4f;

    std::shared_ptr<VertexBuffer> m_vertexBuffer;
    std::shared_ptr<IndexBuffer> m_indexBuffer;
    std::shared_ptr<VertexArray> m_vertexArray;

    void ProcessMesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices,
                     const std::unordered_map<std::string, std::shared_ptr<Texture2D>> &textures,
                     const std::unordered_map<std::string, float> &uniform1f,
                     const std::unordered_map<std::string, glm::vec4> &uniform4f);
};

} // namespace Doodle
