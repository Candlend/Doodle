#pragma once

#include "pch.h"

#include "Project.h"
#include "Singleton.h"

namespace Doodle
{

class DOO_API ProjectManager : public Singleton<ProjectManager>
{
public:
    static std::shared_ptr<Project> CreateProject();

    static std::shared_ptr<Project> LoadProject();

    static std::shared_ptr<Project> CreateProject(const std::string &filepath);

    static std::shared_ptr<Project> LoadProject(const std::string &filepath);

    static std::shared_ptr<Project> GetActiveProject()
    {
        return s_ActiveProject;
    }

private:
    static std::shared_ptr<Project> s_ActiveProject;
};

} // namespace Doodle
