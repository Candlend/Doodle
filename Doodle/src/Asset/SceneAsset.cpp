#include "SceneAsset.h"
#include "SceneManager.h"

namespace Doodle
{

void SceneAsset::Reload()
{
    DeserializeFromFile(m_assetPath);
}
UUID SceneAsset::GetUUID() const
{
    return m_data.UUID;
}
void SceneAsset::Save()
{
    SerializeToFile(m_assetPath);
}
void SceneAsset::SaveAs(const std::filesystem::path &assetPath)
{
    DOO_CORE_ASSERT(assetPath.string().ends_with(GetAssetExtension()), "Invalid file extension");
    m_assetPath = assetPath;
    m_data.UUID = UUID();
    Save();
}

bool SceneAsset::Load(const std::filesystem::path &assetPath)
{
    DOO_CORE_ASSERT(assetPath.string().ends_with(GetAssetExtension()), "Invalid file extension");
    m_assetPath = assetPath;
    return DeserializeFromFile(assetPath);
}
} // namespace Doodle