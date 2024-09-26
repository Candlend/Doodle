#include "ModelAsset.h"

namespace Doodle
{

void ModelAsset::Reload()
{
    DeserializeFromFile(m_assetPath);
}
UUID ModelAsset::GetUUID() const
{
    return m_data.UUID;
}
void ModelAsset::Save()
{
    SerializeToFile(m_assetPath);
}
void ModelAsset::SaveAs(const std::filesystem::path &assetPath)
{
    DOO_CORE_ASSERT(assetPath.string().ends_with(GetAssetExtension()), "Invalid file extension");
    m_assetPath = assetPath;
    m_data.UUID = UUID();
    Save();
}

bool ModelAsset::Load(const std::filesystem::path &assetPath)
{
    DOO_CORE_ASSERT(assetPath.string().ends_with(GetAssetExtension()), "Invalid file extension");
    m_assetPath = assetPath;
    return DeserializeFromFile(assetPath);
}
} // namespace Doodle