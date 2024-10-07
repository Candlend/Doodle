#pragma once

#include "ModelAsset.h"
#include "ModelInfo.h"
#include "pch.h"

#include "Mesh.h"
#include "MeshInfo.h"
#include "Texture.h"
#include <memory>

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
    std::unordered_map<std::string, MeshInfo> MeshInfos;
    std::vector<ModelNode> Children;
};

class DOO_API Model
{
public:
    static std::shared_ptr<Model> Load(const std::filesystem::path &assetPath);
    static std::shared_ptr<Model> Create(const ModelInfo &info);
    ModelNode GetRootNode() const
    {
        return m_root;
    }

    std::shared_ptr<Mesh> GetMesh(unsigned int index) const
    {
        return m_meshes[index];
    }

    ~Model();

protected:
    Model(const ModelInfo &info);
    ModelInfo m_info;
    ModelNode m_root;
    std::vector<std::shared_ptr<Mesh>> m_meshes;
};
} // namespace Doodle