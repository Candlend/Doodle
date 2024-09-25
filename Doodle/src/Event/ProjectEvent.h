#pragma once

#include "Event.h"
#include "Project.h"

namespace Doodle
{

class DOO_API ProjectOpenEvent : public Event
{
public:
    ProjectOpenEvent(Project *project) : m_project(project)
    {
    }

    inline const Project *GetProject() const
    {
        return m_project;
    }

    EVENT_CLASS_TYPE(ProjectOpen)

private:
    Project *m_project;
};

} // namespace Doodle
