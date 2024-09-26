#include "SceneAsset.h"
#include "SceneManager.h"

namespace Doodle
{

void SceneAsset::Reload()
{
    DeserializeFromFile(m_filepath);
}
UUID SceneAsset::GetUUID() const
{
    return m_data.UUID;
}
void SceneAsset::Save()
{
    SerializeToFile(m_filepath);
}
void SceneAsset::SaveAs(const std::filesystem::path &filepath)
{
    m_filepath = filepath;
    m_data.UUID = UUID();
    Save();
}

bool SceneAsset::Load(const std::filesystem::path &filepath)
{
    m_filepath = filepath;
    return DeserializeFromFile(filepath);
}
} // namespace Doodle