
#include "AssetManager.h"

namespace Doodle
{

void AssetManager::LoadAllAssets(const std::filesystem::path &directory)
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

void AssetManager::DeleteUnusedAssets()
{
}

void AssetManager::OnFileModified(const std::filesystem::path &filepath)
{
    auto asset = GetAsset(filepath);
    if (asset)
        asset->Reload();
}

void AssetManager::OnFileErased(const std::filesystem::path &filepath)
{
    RemoveAsset(filepath);
}

void AssetManager::OnFileCreated(const std::filesystem::path &filepath)
{
    LoadAsset<SceneAsset>(filepath);
}

bool AssetManager::OnEvent(ProjectOpenEvent &event)
{
    LoadAllAssets(event.GetProject()->GetDirectory());
    DeleteUnusedAssets();
    return false;
}

void AssetManager::AddAsset(const std::filesystem::path &filepath, std::shared_ptr<Asset> asset)
{
    m_assets[filepath.string()] = asset;
    m_assetsByUUID[asset->GetUUID()] = asset;
    DOO_CORE_INFO("Asset loaded: {0} ({1})", filepath.string(), asset->GetUUID().ToString());
}

void AssetManager::RemoveAsset(const std::filesystem::path &filepath)
{
    auto asset = GetAsset(filepath);
    if (asset)
    {
        m_assetsByUUID.erase(asset->GetUUID());
        m_assets.erase(filepath.string());
    }
}

std::shared_ptr<Asset> AssetManager::GetAsset(const std::filesystem::path &filepath)
{
    if (m_assets.contains(filepath.string()))
    {
        return m_assets[filepath.string()];
    }
    return nullptr;
}

} // namespace Doodle