#pragma once

#include <rfl/Variant.hpp>

#include "Asset.h"
#include "ISerializable.h"
#include "UUID.h"

namespace Doodle
{

struct EntityInfo
{
    UUID UUID;
    std::string Name;
    rfl::Generic::Object Components;
    std::vector<EntityInfo> Children;
};

struct SceneInfo
{
    UUID UUID;
    std::vector<EntityInfo> Entities;
};

class SceneAsset : public Asset, public ISerializable<SceneInfo>
{
    friend class Scene;

public:
    ASSET_TYPE(Scene)
    ASSET_EXTENSION(".dscene")

    void Reload() override;

    UUID GetUUID() const override;

    void Save() override;

    void SaveAs(const std::filesystem::path &assetPath) override;

    bool Load(const std::filesystem::path &assetPath) override;
};

} // namespace Doodle