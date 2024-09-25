#pragma once
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
    void LoadAllAssets(const std::filesystem::path &directory)
    {
        m_fileWatcher = std::make_shared<FileWatcher>(directory);
        std::function<void(std::filesystem::path, FileEventType)> callback = [this](std::filesystem::path filepath,
                                                                                    FileEventType fileEventType) {
            DOO_CORE_DEBUG("File {1}: {0}", filepath.string(), magic_enum::enum_name(fileEventType));
            if (!std::filesystem::is_regular_file(filepath))
            {
                return;
            }
            switch (fileEventType)
            {
            case FileEventType::Modified:
                OnFileModified(filepath);
                break;
            case FileEventType::Erased:
                OnFileErased(filepath);
                break;
            case FileEventType::Created:
                OnFileCreated(filepath);
                break;
            }
        };
        m_fileWatcher->Start(callback);
    }

    void DeleteUnusedAssets()
    {
    }

    void OnFileModified(const std::filesystem::path &filepath)
    {
        auto asset = GetAsset(filepath);
        if (asset)
            asset->Reload();
    }

    void OnFileErased(const std::filesystem::path &filepath)
    {
        RemoveAsset(filepath);
    }

    void OnFileCreated(const std::filesystem::path &filepath)
    {
        bool result = false;
        if (!result)
            result = TryLoadAsset<SceneAsset>(filepath);
    }

    bool OnEvent(ProjectOpenEvent &event) override
    {
        LoadAllAssets(event.GetProject()->GetDirectory());
        DeleteUnusedAssets();
        return false;
    }

    void AddAsset(const std::filesystem::path &filepath, std::shared_ptr<BaseAsset> asset)
    {
        m_assets[filepath.string()] = asset;
        m_assetsByUUID[asset->GetUUID()] = asset;
        DOO_CORE_INFO("Asset loaded: {0} ({1})", filepath.string(), asset->GetUUID().ToString());
    }

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

    void RemoveAsset(const std::filesystem::path &filepath)
    {
        auto asset = GetAsset(filepath);
        if (asset)
        {
            m_assetsByUUID.erase(asset->GetUUID());
            m_assets.erase(filepath.string());
        }
    }

    std::shared_ptr<BaseAsset> GetAsset(const std::filesystem::path &filepath)
    {
        if (m_assets.contains(filepath.string()))
        {
            return m_assets[filepath.string()];
        }
        return nullptr;
    }

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
        AddAsset(filepath, std::dynamic_pointer_cast<BaseAsset>(asset));
        return asset;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<BaseAsset>> m_assets;
    std::unordered_map<UUID, std::shared_ptr<BaseAsset>> m_assetsByUUID;
    std::shared_ptr<FileWatcher> m_fileWatcher;
};

} // namespace Doodle