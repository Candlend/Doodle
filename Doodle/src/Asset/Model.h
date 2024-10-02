#pragma once

#include "ModelAsset.h"
#include "pch.h"

#include "Mesh.h"
#include "Texture.h"
#include <memory>
#include <unordered_map>
#include <vector>

class aiMesh;
class aiNode;
class aiScene;
class aiMaterial;
enum aiTextureType; // NOLINT

namespace Doodle
{
class Entity;

struct MeshInfo
{
    unsigned int MeshIndex;
    std::string Name;
};

struct ModelNode
{
    std::string Name;
    std::unordered_map<std::string, MeshInfo> MeshInfos;
    std::vector<ModelNode> Children;
};

class DOO_API Model
{
public:
    static std::shared_ptr<Model> LoadFromAsset(std::shared_ptr<ModelAsset> asset);
    Model(std::shared_ptr<ModelAsset> asset);
    ModelNode GetRootNode() const
    {
        return m_root;
    }

    ~Model();

private:
    std::shared_ptr<ModelAsset> m_asset;
    ModelNode m_root;
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_loadedTextures;
};
} // namespace Doodle