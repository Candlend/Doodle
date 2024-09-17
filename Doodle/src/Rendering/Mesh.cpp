#include "VertexBuffer.h"
#include "pch.h"
#include <assimp/DefaultLogger.hpp>
#include <assimp/Importer.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glad/glad.h>

#include "Mesh.h"
#include "VertexArray.h"

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
    DOO_CORE_ASSERT(mesh->HasTangentsAndBitangents(), "Meshes require tangents and bitangents.");
    DOO_CORE_ASSERT(mesh->HasTextureCoords(0), "Meshes require texture coordinates.");

    m_vertices.reserve(mesh->mNumVertices);

    // Extract vertices from model
    for (size_t i = 0; i < mesh->mNumVertices; i++)
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
            vertex.TexCoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};

        m_vertices.push_back(vertex);
    }

    // Create Vertex Buffer Object
    m_vertexBuffer = VertexBuffer::Create(m_vertices.data(), m_vertices.size() * sizeof(Vertex));
    m_vertexBuffer->PushElement("a_Position", VertexDataType::Vec3);
    m_vertexBuffer->PushElement("a_Normal", VertexDataType::Vec3);
    m_vertexBuffer->PushElement("a_Tangent", VertexDataType::Vec3);
    m_vertexBuffer->PushElement("a_Binormal", VertexDataType::Vec3);
    m_vertexBuffer->PushElement("a_TexCoord", VertexDataType::Vec2);

    // Extract indices from model
    m_indices.reserve(mesh->mNumFaces * 3); // Each face has 3 indices
    for (size_t i = 0; i < mesh->mNumFaces; i++)
    {
        DOO_CORE_ASSERT(mesh->mFaces[i].mNumIndices == 3, "Must have 3 indices.");
        for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
        {
            m_indices.push_back(mesh->mFaces[i].mIndices[j]);
        }
    }

    // Create Index Buffer Object
    m_indexBuffer = IndexBuffer::Create(m_indices.data(), m_indices.size() * sizeof(uint32_t));

    // Create Vertex Array Object
    m_vertexArray = VertexArray::Create();
    m_vertexArray->AddVertexBuffer(m_vertexBuffer);
    m_vertexArray->SetIndexBuffer(m_indexBuffer);
}

Mesh::~Mesh()
{
}

void Mesh::Render()
{
    m_vertexArray->Render();
}

std::shared_ptr<Mesh> Mesh::GetQuad()
{
    static std::shared_ptr<Mesh> s_Quad = nullptr;
    if (!s_Quad)
    {
        s_Quad = std::make_shared<Mesh>("assets/models/quad.obj");
    }
    return s_Quad;
}

std::shared_ptr<Mesh> Mesh::GetCube()
{
    static std::shared_ptr<Mesh> s_Cube = nullptr;
    if (!s_Cube)
    {
        s_Cube = std::make_shared<Mesh>("assets/models/cube.obj");
    }
    return s_Cube;
}

std::shared_ptr<Mesh> Mesh::GetSphere()
{
    static std::shared_ptr<Mesh> s_Sphere = nullptr;
    if (!s_Sphere)
    {
        s_Sphere = std::make_shared<Mesh>("assets/models/sphere.obj");
    }
    return s_Sphere;
}

std::shared_ptr<Mesh> Mesh::GetPlane()
{
    static std::shared_ptr<Mesh> s_Plane = nullptr;
    if (!s_Plane)
    {
        s_Plane = std::make_shared<Mesh>("assets/models/plane.obj");
    }
    return s_Plane;
}

std::shared_ptr<Mesh> Mesh::Create(const std::string &filename)
{
    return std::make_shared<Mesh>(filename);
}

} // namespace Doodle
