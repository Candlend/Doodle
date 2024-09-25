#pragma once

#include "pch.h"
#include <filesystem>
#include <rfl/Rename.hpp>

#include "ISerializable.h"
#include "UUID.h"

namespace Doodle
{

struct ProjectSettings
{
    std::string Name = "untitledProject";
    rfl::Rename<"StartScene", UUID> StartSceneUUID = UUID::Nil();
};

class DOO_API Project : ISerializable<ProjectSettings>
{
public:
    Project(const std::filesystem::path &filepath = "");

    static std::shared_ptr<Project> Create(const std::filesystem::path &filepath);
    static std::shared_ptr<Project> Load(const std::filesystem::path &filepath);

    void Save();

    bool Reload();

    void Apply();

    std::filesystem::path GetDirectory() const
    {
        return m_directory;
    }

    std::filesystem::path GetFilepath() const
    {
        return m_filepath;
    }

    std::string GetName() const
    {
        return m_data.Name;
    }

private:
    std::filesystem::path m_directory;
    std::filesystem::path m_filepath;
};

} // namespace Doodle
