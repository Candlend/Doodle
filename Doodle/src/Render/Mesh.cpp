#include "pch.h"
#include <assimp/DefaultLogger.hpp>
#include <assimp/Importer.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glad/glad.h>

#include "Mesh.h"


namespace Doodle
{

namespace
{
const unsigned int IMPORT_FLAGS = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_SortByPType |
                                  aiProcess_PreTransformVertices | aiProcess_GenNormals | aiProcess_GenUVCoords |
                                  aiProcess_OptimizeMeshes | aiProcess_Debone | aiProcess_ValidateDataStructure;
}

struct LogStream : public Assimp::LogStream
{
    static void Initialize()
    {
        if (Assimp::DefaultLogger::isNullLogger())
        {
            Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
            Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Err | Assimp::Logger::Warn);
        }
    }

    void write(const char *message) override
    {
        DOO_CORE_ERROR("Assimp error: {0}", message);
    }
};

Mesh::Mesh(const std::string &filename) : m_filePath(filename)
{
    LogStream::Initialize();

    DOO_CORE_INFO("Loading mesh: {0}", filename.c_str());

    Assimp::Importer importer;

    const aiScene *scene = importer.ReadFile(filename, IMPORT_FLAGS);
    if (!scene || !scene->HasMeshes())
        DOO_CORE_ERROR("Failed to load mesh file: {0}", filename);

    aiMesh *mesh = scene->mMeshes[0];

    DOO_CORE_ASSERT(mesh->HasPositions(), "Meshes require positions.");
    DOO_CORE_ASSERT(mesh->HasNormals(), "Meshes require normals.");

    m_vertices.reserve(mesh->mNumVertices);

    // Extract vertices from model
    for (size_t i = 0; i < m_vertices.capacity(); i++)
    {
        Vertex vertex;
        vertex.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
        vertex.Normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};

        if (mesh->HasTangentsAndBitangents())
        {
            vertex.Tangent = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
            vertex.Binormal = {mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
        }

        if (mesh->HasTextureCoords(0))
            vertex.Texcoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        m_vertices.push_back(vertex);
    }

    m_vertexBuffer = VertexBuffer::Create(m_vertices.data(), m_vertices.size() * sizeof(Vertex));

    // Extract indices from model
    m_indices.reserve(mesh->mNumFaces);
    for (size_t i = 0; i < m_indices.capacity(); i++)
    {
        DOO_CORE_ASSERT(mesh->mFaces[i].mNumIndices == 3, "Must have 3 indices.");
        m_indices.push_back({mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2]});
    }

    m_indexBuffer = IndexBuffer::Create(m_indices.data(), m_indices.size() * sizeof(Index));
}

Mesh::~Mesh()
{
}

void Mesh::Render()
{
    // TODO: Sort this out
    m_vertexBuffer->Bind();
    m_indexBuffer->Bind();
    std::function func = []()
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void *>(offsetof(Vertex, Position)));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void *>(offsetof(Vertex, Normal)));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void *>(offsetof(Vertex, Tangent)));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void *>(offsetof(Vertex, Binormal)));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void *>(offsetof(Vertex, Texcoord)));
    };
    Renderer::DrawIndexed(m_indexBuffer->GetCount());
}

} // namespace Doodle
