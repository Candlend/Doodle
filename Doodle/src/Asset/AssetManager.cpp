
#include "AssetManager.h"
#include "FileSystem.h"
#include "Log.h"
#include "ModelAsset.h"
#include <filesystem>

namespace Doodle
{

void AssetManager::LoadAllAssets(const std::filesystem::path &directory)
{
    m_fileWatcher = std::make_shared<FileWatcher>(directory);
    std::function<void(std::filesystem::path, FileEventType)> callback = [this](std::filesystem::path filepath,
                                                                                FileEventType fileEventType) {
        if (std::filesystem::is_directory(filepath))
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

void AssetManager::DeleteUnusedAssets()
{
    for (auto &[assetPath, asset] : m_assets)
    {
        if (auto fileAsset = std::dynamic_pointer_cast<FileAsset>(asset))
        {
            if (!fileAsset->IsValid())
            {
                RemoveAsset(assetPath);
            }
        }
    }
}

void AssetManager::OnFileModified(const std::filesystem::path &filepath)
{
    auto asset = GetAsset(filepath);
    if (asset)
    {
        DOO_CORE_INFO("asset modified: {0}", filepath.string());
        asset->Reload();
    }
}

void AssetManager::OnFileErased(const std::filesystem::path &filepath)
{
    if (m_fileToFileAsset.contains(filepath))
    {
        auto assetPath = m_fileToFileAsset[filepath];
        RemoveAsset(assetPath);
        FileSystem::DeleteFile(assetPath);
        m_fileToFileAsset.erase(filepath);
        return;
    }
    if (auto asset = GetAsset(filepath))
    {
        if (auto fileAsset = std::dynamic_pointer_cast<FileAsset>(asset))
        {
            if (fileAsset->IsValid())
            {
                fileAsset->Save();
            }
        }
    }
    RemoveAsset(filepath);
}

void AssetManager::OnFileCreated(const std::filesystem::path &filepath)
{
    LoadFileAsset<ModelAsset>(filepath);
    LoadNativeAsset<SceneAsset>(filepath);
}

bool AssetManager::OnEvent(ProjectOpenEvent &event)
{
    LoadAllAssets(event.GetProject()->GetDirectory());
    DeleteUnusedAssets();
    return false;
}

void AssetManager::AddAsset(const std::filesystem::path &assetPath, std::shared_ptr<Asset> asset)
{
    m_assets[assetPath.string()] = asset;
    m_assetsByUUID[asset->GetUUID()] = asset;
}

void AssetManager::RemoveAsset(const std::filesystem::path &assetPath)
{
    auto asset = GetAsset(assetPath);
    if (asset)
    {
        DOO_CORE_INFO("Asset erased: {0} ({1})", assetPath.string(), asset->GetUUID().ToString());
        m_assetsByUUID.erase(asset->GetUUID());
        m_assets.erase(assetPath.string());
    }
}

std::shared_ptr<Asset> AssetManager::GetAsset(const std::filesystem::path &assetPath)
{
    if (m_assets.contains(assetPath.string()))
    {
        return m_assets[assetPath.string()];
    }
    return nullptr;
}

} // namespace Doodle