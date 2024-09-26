#pragma once
#include "Core.h"
#include "Log.h"
#include "pch.h"
#include <filesystem>
#include <magic_enum.hpp>
#include <type_traits>

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

    void AddAsset(const std::filesystem::path &assetPath, std::shared_ptr<Asset> asset);

    void RemoveAsset(const std::filesystem::path &assetPath);

    std::shared_ptr<Asset> GetAsset(const std::filesystem::path &assetPath);

    template <typename T> std::shared_ptr<T> GetAsset(const std::filesystem::path &assetPath)
    {
        if (m_assets.contains(assetPath))
        {
            return std::dynamic_pointer_cast<T>(m_assets[assetPath]);
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

    template <typename T> std::shared_ptr<T> Create(const std::filesystem::path &assetPath)
    {
        static_assert(!std::is_base_of_v<FileAsset, T>, "FileAsset cannot be created");
        if (!assetPath.string().ends_with(T::GetStaticExtension()))
        {
            DOO_CORE_ERROR("Invalid file extension: {0}", assetPath.string());
            return nullptr;
        }
        auto asset = std::make_shared<T>();

        asset->SaveAs(assetPath);
        AddAsset(assetPath, std::dynamic_pointer_cast<Asset>(asset));
        DOO_CORE_INFO("NativeAsset created: {0} ({1})", assetPath.string(), asset->GetUUID().ToString());
        return asset;
    }

private:
    template <typename T> bool TryLoadFileAsset(const std::filesystem::path &filepath)
    {
        if (!T::IsSupported(filepath))
        {
            return false;
        }
        else
        {
            auto assetPath = T::GetAssetPathByFilepath(filepath);
            m_fileToFileAsset[filepath] = assetPath;
            if (!std::filesystem::exists(assetPath))
            {
                auto asset = std::make_shared<T>(filepath);
                asset->Save();
                AddAsset(assetPath, asset);
                DOO_CORE_INFO("FileAsset created: {0} ({1})", assetPath.string(), asset->GetUUID().ToString());
            }
            else
            {
                auto asset = std::make_shared<T>(filepath);
                asset->Load(assetPath);
                AddAsset(assetPath, asset);
                DOO_CORE_INFO("FileAsset loaded: {0} ({1})", assetPath.string(), asset->GetUUID().ToString());
            }
            return true;
        }
    }

    template <typename... Assets> void LoadFileAsset(const std::filesystem::path &filepath)
    {
        (TryLoadFileAsset<Assets>(filepath) || ...);
    }

    template <typename T> bool TryLoadNativeAsset(const std::filesystem::path &assetPath)
    {
        if (m_assets.contains(assetPath))
        {
            return true;
        }
        auto extension = assetPath.extension().string();
        if (!assetPath.string().ends_with(T::GetStaticExtension()))
        {
            return false;
        }
        auto asset = std::make_shared<T>();
        if (asset->Load(assetPath.string()))
        {
            AddAsset(assetPath, asset);
            DOO_CORE_INFO("NativeAsset loaded: {0} ({1})", assetPath.string(), asset->GetUUID().ToString());
            return true;
        };
        return false;
    }

    template <typename... Assets> void LoadNativeAsset(const std::filesystem::path &assetPath)
    {
        (TryLoadNativeAsset<Assets>(assetPath) || ...);
    }
    std::unordered_map<std::filesystem::path, std::filesystem::path> m_fileToFileAsset;
    std::unordered_map<std::filesystem::path, std::shared_ptr<Asset>> m_assets;
    std::unordered_map<UUID, std::shared_ptr<Asset>> m_assetsByUUID;
    std::shared_ptr<FileWatcher> m_fileWatcher;
};

} // namespace Doodle