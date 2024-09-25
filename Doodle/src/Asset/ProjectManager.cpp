
#include "ProjectManager.h"
#include "FileSystem.h"

namespace Doodle
{

std::shared_ptr<Project> ProjectManager::s_ActiveProject = nullptr;

std::shared_ptr<Project> ProjectManager::CreateProject()
{
    std::filesystem::path filepath = FileSystem::SaveFileDialog({{"Doodle Project", "doodle"}});
    if (filepath.empty())
        return nullptr;
    return CreateProject(filepath.string());
}

std::shared_ptr<Project> ProjectManager::LoadProject()
{
    std::filesystem::path filepath = FileSystem::OpenFileDialog({{"Doodle Project", "doodle"}});
    if (filepath.empty())
        return nullptr;
    return LoadProject(filepath.string());
}

std::shared_ptr<Project> ProjectManager::CreateProject(const std::string &filepath)
{
    DOO_CORE_INFO("Creating project: {0}", filepath);
    s_ActiveProject = Project::Create(filepath);
    return s_ActiveProject;
}

std::shared_ptr<Project> ProjectManager::LoadProject(const std::string &filepath)
{
    DOO_CORE_INFO("Loading project: {0}", filepath);
    s_ActiveProject = Project::Load(filepath);
    return s_ActiveProject;
}

} // namespace Doodle
