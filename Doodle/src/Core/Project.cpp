#include <memory>
#include <rfl.hpp>
#include <rfl/json.hpp>
#include <rfl/yaml.hpp>

#include "AssetManager.h"
#include "EventManager.h"
#include "FileSystem.h"
#include "Project.h"
#include "ProjectEvent.h"
#include "SceneAsset.h"
#include "SceneManager.h"

namespace Doodle
{

Project::Project(const std::filesystem::path &filepath)
{
    m_directory = std::filesystem::absolute(filepath.parent_path());
    m_filepath = filepath;
}

std::shared_ptr<Project> Project::Create(const std::filesystem::path &filepath)
{
    auto newProject = std::make_shared<Project>(filepath);
    ProjectInfo info;
    auto sceneAsset =
        AssetManager::Get()->Create<SceneAsset>(newProject->GetDirectory() / "scenes" / "sampleScene.dscene");
    info.StartSceneUUID = sceneAsset->GetUUID();
    newProject->SetInfo(info);
    newProject->Apply();
    newProject->Save();
    return newProject;
}

std::shared_ptr<Project> Project::Load(const std::filesystem::path &filepath)
{
    auto newProject = std::make_shared<Project>(filepath);
    if (newProject->DeserializeFromFile(filepath))
    {
        newProject->Apply();
        return newProject;
    }
    return nullptr;
}

void Project::Save()
{
    SerializeToFile(m_filepath);
}

bool Project::Reload()
{
    if (DeserializeFromFile(m_filepath))
    {
        Apply();
        return true;
    }
    return false;
}

void Project::Apply()
{
    DOO_CORE_INFO(rfl::yaml::write(m_info));
    EventManager::Get()->Dispatch<ProjectOpenEvent>(this);
    auto startSceneAsset = AssetManager::Get()->GetAsset<SceneAsset>(m_info.StartSceneUUID.value());
    if (startSceneAsset)
    {
        SceneManager::Get()->LoadScene(startSceneAsset->GetAssetPath());
    }
    else
    {
        DOO_CORE_ERROR("Failed to load start scene {0}", m_info.StartSceneUUID.value().ToString());
    }
}

} // namespace Doodle
