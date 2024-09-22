#include <assimp/DefaultLogger.hpp>
#include <assimp/Importer.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Entity.h"
#include "Log.h"
#include "Mesh.h"
#include "Model.h"
#include "Texture.h"
#include "TextureParams.h"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/types.h"

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

void Model::LoadTexture(std::unordered_map<std::string, std::shared_ptr<Texture2D>> &textures, aiMaterial *material,
                        std::string name, aiTextureType type, int index, TextureParams params)
{
    if (textures.contains(name))
        return;
    aiString str;
    aiReturn result = material->GetTexture(type, index, &str);
    if (result != aiReturn_SUCCESS)
        return;
    std::filesystem::path texturePath = std::filesystem::path(m_directory) / str.C_Str();
    DOO_CORE_INFO("Loading texture: {0}", texturePath.string());
    if (m_loadedTextures.contains(texturePath.string()))
    {
        textures[name] = m_loadedTextures[texturePath.string()];
        return;
    }
    textures[name] = Texture2D::Create(texturePath.string(), params);
    m_loadedTextures[texturePath.string()] = textures[name];
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

    std::unordered_map<std::string, std::shared_ptr<Texture2D>> textures;
    TextureParams srgbParams;
    srgbParams.Format = TextureFormat::SRGB8ALPHA8;
    LoadTexture(textures, material, "u_AlbedoTexture", AI_MATKEY_BASE_COLOR_TEXTURE, srgbParams);
    LoadTexture(textures, material, "u_AlbedoTexture", aiTextureType_DIFFUSE, 0, srgbParams);
    LoadTexture(textures, material, "u_NormalTexture", aiTextureType_NORMALS);
    LoadTexture(textures, material, "u_MetalnessTexture", AI_MATKEY_METALLIC_TEXTURE);
    LoadTexture(textures, material, "u_RoughnessTexture", AI_MATKEY_ROUGHNESS_TEXTURE);
    TextureParams invertParams;
    invertParams.InvertColor = true;
    LoadTexture(textures, material, "u_RoughnessTexture", aiTextureType_SPECULAR, 0, invertParams);
    LoadTexture(textures, material, "u_RoughnessTexture", aiTextureType_SHININESS, 0, invertParams);

    std::unordered_map<std::string, float> uniform1f;
    std::unordered_map<std::string, glm::vec4> uniform4f;

    aiColor3D color(1.0f);
    float alpha = 1.0f;
    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS)
    {
        uniform4f["u_AlbedoColor"] = {color.r, color.g, color.b, alpha};
    }
    if (material->Get(AI_MATKEY_OPACITY, alpha) == aiReturn_SUCCESS)
    {
        uniform4f["u_AlbedoColor"].w = alpha;
    }

    float roughness, metallic;
    if (material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == aiReturn_SUCCESS)
    {
        uniform1f["u_Roughness"] = roughness;
    }
    if (material->Get(AI_MATKEY_REFLECTIVITY, metallic) == aiReturn_SUCCESS)
    {
        uniform1f["u_Metallic"] = metallic;
    }
    if (textures.contains("u_AlbedoTexture"))
    {
        uniform4f["u_AlbedoColor"] = {1.0f, 1.0f, 1.0f, alpha};
    }
    if (textures.contains("u_RoughnessTexture"))
    {
        uniform1f["u_Roughness"] = 1.0f;
    }
    if (textures.contains("u_MetalnessTexture"))
    {
        uniform1f["u_Metallic"] = 1.0f;
    }

    return std::make_shared<Mesh>(vertices, indices, textures, uniform1f, uniform4f);
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