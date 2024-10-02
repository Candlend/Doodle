#pragma once

#include <rfl/Variant.hpp>

#include "Asset.h"
#include "ISerializable.h"
#include "UUID.h"

namespace Doodle
{

struct ModelInfo
{
    UUID UUID;
};

class ModelAsset : public FileAsset, public ISerializable<ModelInfo>
{
    friend class Model;

public:
    ASSET_TYPE(Model)
    ASSET_EXTENSION(".dmodel")
    BIND_FILE_FILTER({"Wavefront OBJ", "obj"}, {"Autodesk FBX", "fbx"}, {"GL Transmission", "gltf,glb"})

    ModelAsset(const std::filesystem::path &filepath) : FileAsset(filepath)
    {
        auto directory = filepath.parent_path();
        m_assetPath = directory / (m_bindFilename + GetAssetExtension());
    }

    void Reload() override;

    UUID GetUUID() const override;

    void Save() override;

    void SaveAs(const std::filesystem::path &assetPath) override;

    bool Load(const std::filesystem::path &assetPath) override;
};

} // namespace Doodle