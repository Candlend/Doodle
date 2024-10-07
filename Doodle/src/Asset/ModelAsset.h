#pragma once

#include <rfl/Variant.hpp>

#include "Asset.h"
#include "ISerializable.h"
#include "ModelInfo.h"
#include "UUID.h"

namespace Doodle
{

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
}; /*  */

} // namespace Doodle