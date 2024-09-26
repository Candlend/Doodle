#pragma once
#include "Core.h"
#include "pch.h"
#include <filesystem>
#include <magic_enum.hpp>

#include "Asset.h"
#include "EventManager.h"
#include "FileWatcher.h"
#include "IEventHandler.h"
#include "ProjectEvent.h"
#include "ProjectManager.h"
#include "SceneAsset.h"
#include "Singleton.h"
#include "UUID.h"

namespace Doodle
{

class DOO_API AssetManager : public Singleton<AssetManager>, public IEventHandler<ProjectOpenEvent>
{
public:
    void LoadAllAssets(const std::filesystem::path &directory);

    void DeleteUnusedAssets();

    void OnFileModified(const std::filesystem::path &filepath);

    void OnFileErased(const std::filesystem::path &filepath);

    void OnFileCreated(const std::filesystem::path &filepath);

    bool OnEvent(ProjectOpenEvent &event) override;

    void AddAsset(const std::filesystem::path &filepath, std::shared_ptr<Asset> asset);

    void RemoveAsset(const std::filesystem::path &filepath);

    std::shared_ptr<Asset> GetAsset(const std::filesystem::path &filepath);

    template <typename T> std::shared_ptr<T> GetAsset(const std::string &name)
    {
        if (m_assets.contains(name))
        {
            return std::dynamic_pointer_cast<T>(m_assets[name]);
        }
        return nullptr;
    }

    template <typename T> std::shared_ptr<T> GetAsset(const UUID &uuid)
    {
        if (m_assetsByUUID.contains(uuid))
        {
            return std::dynamic_pointer_cast<T>(m_assetsByUUID[uuid]);
        }
        return nullptr;
    }

    template <typename T> std::shared_ptr<T> Create(const std::filesystem::path &filepath)
    {
        auto extension = filepath.extension().string().substr(1);
        if (extension != T::GetStaticExtension())
        {
            DOO_CORE_ERROR("Invalid file extension: {0}", extension);
            return nullptr;
        }
        auto asset = std::make_shared<T>(filepath);
        asset->Save();
        AddAsset(filepath, std::dynamic_pointer_cast<Asset>(asset));
        return asset;
    }

private:
    template <typename T> bool TryLoadAsset(const std::filesystem::path &filepath)
    {
        auto extension = filepath.extension().string().substr(1);
        if (extension != T::GetStaticExtension())
        {
            return false;
        }
        auto asset = std::make_shared<T>();
        if (asset->Load(filepath.string()))
        {
            AddAsset(filepath, asset);
            return true;
        };
        return false;
    }

    template <typename... Assets> void LoadAsset(const std::filesystem::path &filepath)
    {
        (TryLoadAsset<Assets>(filepath) || ...);
    }
    std::unordered_map<std::string, std::shared_ptr<Asset>> m_assets;
    std::unordered_map<UUID, std::shared_ptr<Asset>> m_assetsByUUID;
    std::shared_ptr<FileWatcher> m_fileWatcher;
};

} // namespace Doodle