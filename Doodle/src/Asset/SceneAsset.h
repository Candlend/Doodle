#pragma once

#include <rfl/Variant.hpp>

#include "Asset.h"
#include "ISerializable.h"
#include "Scene.h"
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

class SceneAsset : public Asset<Scene>, public ISerializable<SceneInfo>
{
public:
    ASSET_TYPE(Scene, dscene)

    SceneAsset(const std::filesystem::path &filepath = "")
    {
        m_filepath = filepath;
    }
    std::shared_ptr<Scene> CreateInstance() override;

    void Reload() override;

    UUID GetUUID() const override;

    void Save() override;

    void SaveAs(const std::filesystem::path &filepath) override;

    bool Load(const std::filesystem::path &filepath) override;
};

} // namespace Doodle