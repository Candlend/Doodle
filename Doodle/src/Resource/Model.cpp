#include <assimp/DefaultLogger.hpp>
#include <assimp/Importer.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/types.h>
#include <filesystem>
#include <memory>

#include "AssetManager.h"
#include "Mesh.h"
#include "Model.h"
#include "ModelAsset.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "Utils.h"

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

// void Model::LoadTexture(std::unordered_map<std::string, std::shared_ptr<Texture2D>> &textures, aiMaterial *material,
//                         std::string name, aiTextureType type, int index, TextureSpecification spec)
// {
//     if (textures.contains(name))
//         return;
//     aiString str;
//     aiReturn result = material->GetTexture(type, index, &str);
//     if (result != aiReturn_SUCCESS)
//         return;
//     std::filesystem::path texturePath = std::filesystem::path(m_directory) / str.C_Str();
//     DOO_CORE_INFO("Loading texture: {0}", texturePath.string());
//     if (m_loadedTextures.contains(texturePath.string()))
//     {
//         textures[name] = m_loadedTextures[texturePath.string()];
//         return;
//     }
//     textures[name] = Texture2D::Create(texturePath.string(), spec);
//     m_loadedTextures[texturePath.string()] = textures[name];
// }

Model::Model(const ModelInfo &info)
{
    m_info = info;
    auto asset = AssetManager::Get()->GetAsset<ModelAsset>(info.UUID);
    auto filepath = asset->GetBindFilepath();
    LogStream::Initialize();
    DOO_CORE_INFO("Loading model: {0}", filepath.string());
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filepath.string(), aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                                                    aiProcess_CalcTangentSpace);
    m_meshes.reserve(scene->mNumMeshes);

    auto loadMesh = [this, scene](aiMesh *mesh) {
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
        return Mesh::Create(vertices, indices);
    };

    for (size_t i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[i];
        m_meshes.push_back(loadMesh(mesh));
    }

    std::function<ModelNode(aiNode *, const aiScene *)> processNode = [this, &processNode](aiNode *node,
                                                                                           const aiScene *scene) {
        ModelNode modelNode{node->mName.C_Str()};
        // process all the node's meshes (if any)
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            std::string name = material->GetName().C_Str();
            MeshInfo meshInfo{m_info.UUID, i, name};
        }
        // then do the same for each of its children
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            auto child = processNode(node->mChildren[i], scene);
            modelNode.Children.push_back(child);
        }
        return modelNode;
    };

    m_root = processNode(scene->mRootNode, scene);
}

std::shared_ptr<Model> Model::Load(const std::filesystem::path &assetPath)
{
    auto asset = AssetManager::Get()->GetAsset(assetPath);
    return ResourceManager::Get()->Load<Model>(asset->GetUUID());
}

std::shared_ptr<Model> Model::Create(const ModelInfo &info)
{
    struct ModelWrapper : public Model
    {
        ModelWrapper(const ModelInfo &info) : Model(info) {};
    };

    return std::make_shared<ModelWrapper>(info);
}

Model::~Model()
{
}

std::shared_ptr<Model> Create(const ModelInfo &info)
{
    struct ModelWrapper : public Model
    {
        ModelWrapper(const ModelInfo &info) : Model(info) {};
    };

    return std::make_shared<ModelWrapper>(info);
}

} // namespace Doodle