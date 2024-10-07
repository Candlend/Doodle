#pragma once

#include "pch.h"
#include <filesystem>
#include <rfl.hpp>
#include <rfl/json.hpp>

#include "FileSystem.h"

namespace Doodle
{

template <typename T> class DOO_API ISerializable
{

public:
    std::string Serialize()
    {
        return rfl::json::write(m_info, YYJSON_WRITE_PRETTY);
    }

    void SerializeToFile(const std::string &path)
    {
        auto directory = std::filesystem::absolute(std::filesystem::path(path).parent_path());
        FileSystem::CreateDirectory(directory);
        std::ofstream file(path);
        file << Serialize();
        file.close();
    }

    void SerializeToFile(const std::filesystem::path &path)
    {
        SerializeToFile(path.string());
    }

    bool Deserialize(const std::string &json)
    {
        auto result = rfl::json::read<T>(json);
        if (result)
        {
            SetInfo(result.value());
            return true;
        }
        else
        {
            auto err = result.error().value();
            DOO_CORE_ERROR("Failed to deserialize JSON: {0}", err.what());
            return false;
        }
    }

    bool DeserializeFromFile(const std::string &path)
    {
        if (!std::filesystem::exists(path))
        {
            DOO_CORE_ERROR("File does not exist: {0}", path);
            return false;
        }
        if (std::filesystem::is_directory(path))
        {
            DOO_CORE_ERROR("Path is a directory: {0}", path);
            return false;
        }
        std::ifstream file(path);
        std::string json((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        return Deserialize(json);
    }

    bool DeserializeFromFile(const std::filesystem::path &path)
    {
        return DeserializeFromFile(path.string());
    }

    T GetInfo() const
    {
        return m_info;
    }

    void SetInfo(const T &info)
    {
        m_info = info;
    }

protected:
    T m_info;
};

} // namespace Doodle