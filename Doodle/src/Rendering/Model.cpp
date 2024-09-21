#include <assimp/DefaultLogger.hpp>
#include <assimp/Importer.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <filesystem>
#include <memory>
#include <string>

#include "Entity.h"
#include "Mesh.h"
#include "Model.h"
#include "assimp/material.h"
#include "assimp/mesh.h"

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

namespace Doodle
{

void Model::LoadTexture(aiMaterial *material, aiTextureType type, std::string name)
{
    for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
    {
        aiString str;
        material->GetTexture(type, i, &str);
        std::filesystem::path texturePath = std::filesystem::path(m_directory) / str.C_Str();
        if (m_loadedTextures.contains(texturePath.string()))
            continue;
        m_textures[name] = Texture2D::Create(texturePath.string());
        break;
    }
}

std::shared_ptr<Mesh> Model::LoadMesh(const aiMesh *mesh, const aiScene *scene)
{
    DOO_CORE_ASSERT(mesh->HasPositions(), "Meshes require positions.");
    DOO_CORE_ASSERT(mesh->HasNormals(), "Meshes require normals.");
    DOO_CORE_ASSERT(mesh->HasTangentsAndBitangents(), "Meshes require tangents and bitangents.");
    DOO_CORE_ASSERT(mesh->HasTextureCoords(0), "Meshes require texture coordinates.");

    std::vector<Vertex> vertices;
    vertices.reserve(mesh->mNumVertices);

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

        vertices.push_back(vertex);
    }

    std::vector<uint32_t> indices;
    // Extract indices from model
    indices.reserve(mesh->mNumFaces * 3); // Each face has 3 indices
    for (size_t i = 0; i < mesh->mNumFaces; i++)
    {
        DOO_CORE_ASSERT(mesh->mFaces[i].mNumIndices == 3, "Must have 3 indices.");
        for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
        {
            indices.push_back(mesh->mFaces[i].mIndices[j]);
        }
    }

    // process the mesh
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    LoadTexture(material, aiTextureType_DIFFUSE, "u_AlbedoTexture");
    LoadTexture(material, aiTextureType_NORMALS, "u_NormalTexture");
    LoadTexture(material, aiTextureType_METALNESS, "u_MetalnessTexture");
    LoadTexture(material, aiTextureType_DIFFUSE_ROUGHNESS, "u_RoughnessTexture");

    return std::make_shared<Mesh>(vertices, indices, m_textures);
}

ModelNode Model::ProcessNode(aiNode *node, const aiScene *scene)
{
    ModelNode modelNode{node->mName.C_Str()};
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::string name = material->GetName().C_Str();
        if (modelNode.Meshes.contains(name))
        {
            name += std::string("_") + std::to_string(i);
        }
        modelNode.Meshes[name] = LoadMesh(mesh, scene);
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        auto child = ProcessNode(node->mChildren[i], scene);
        modelNode.Children.push_back(child);
    }
    return modelNode;
}

std::shared_ptr<Model> Model::Create(const std::string &filepath)
{
    return std::make_shared<Model>(filepath);
}

Model::Model(const std::string &filepath)
{
    LogStream::Initialize();
    DOO_CORE_INFO("Loading model: {0}", filepath.c_str());
    Assimp::Importer importer;
    const aiScene *scene =
        importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        DOO_CORE_ERROR("ERROR::ASSIMP::{0}", importer.GetErrorString());
        return;
    }
    // retrieve the directory path of the filepath
    m_filepath = filepath;
    m_directory = filepath.substr(0, filepath.find_last_of('/'));

    // process ASSIMP's root node recursively
    m_root = ProcessNode(scene->mRootNode, scene);
}

} // namespace Doodle