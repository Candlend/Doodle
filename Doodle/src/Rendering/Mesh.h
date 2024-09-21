#pragma once

#include "pch.h"
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>

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
         const std::unordered_map<std::string, std::shared_ptr<Texture2D>> &textures);
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

private:
    std::string m_filepath;
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_textures;

    std::shared_ptr<VertexBuffer> m_vertexBuffer;
    std::shared_ptr<IndexBuffer> m_indexBuffer;
    std::shared_ptr<VertexArray> m_vertexArray; // 新增：VAO

    void ProcessMesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices,
                     const std::unordered_map<std::string, std::shared_ptr<Texture2D>> &textures);
};

} // namespace Doodle
