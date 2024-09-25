#pragma once

#include <rfl/Variant.hpp>

#include "Asset.h"
#include "ISerializable.h"
#include "Scene.h"
#include "SceneManager.h"
#include "UUID.h"

namespace Doodle
{

class SceneAsset : public Asset<Scene>, public ISerializable<SceneInfo>
{
public:
    ASSET_TYPE(Scene, dscene)

    std::shared_ptr<Scene> CreateInstance() override
    {
        auto scene = SceneManager::Get()->LoadScene(m_data);
        return scene;
    };

    void Reload() override
    {
        DeserializeFromFile(m_filepath);
    }

    UUID GetUUID() const override
    {
        return m_data.UUID;
    }

    void Save()
    {
        SerializeToFile(m_filepath);
    }

    void SaveAs(const std::string &filepath)
    {
        m_filepath = filepath;
        m_data.UUID = UUID();
        Save();
    }
};

} // namespace Doodle