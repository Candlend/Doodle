#pragma once

#include "TextureParams.h"
#include "pch.h"
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "Material.h"
#include "Mesh.h"

class aiMesh;
class aiNode;
class aiScene;
class aiMaterial;
enum aiTextureType; // NOLINT

namespace Doodle
{
class Entity;

struct ModelNode
{
    std::string Name;
    std::unordered_map<std::string, std::shared_ptr<Mesh>> Meshes;
    std::vector<ModelNode> Children;
};

class DOO_API Model
{
public:
    static std::shared_ptr<Model> Create(const std::string &filepath);
    Model(const std::string &filepath);
    ModelNode GetRootNode() const
    {
        return m_root;
    }

private:
    std::string m_filepath;
    std::string m_directory;
    ModelNode m_root;
    std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_loadedTextures;

    ModelNode ProcessNode(aiNode *node, const aiScene *scene);

    void LoadTexture(std::unordered_map<std::string, std::shared_ptr<Texture2D>> &textures, aiMaterial *material,
                     std::string name, aiTextureType type, int index = 0, TextureParams params = TextureParams());

    std::shared_ptr<Mesh> LoadMesh(const aiMesh *mesh, const aiScene *scene);
};
} // namespace Doodle