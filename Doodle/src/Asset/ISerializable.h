#pragma once

#include "pch.h"
#include <rfl.hpp>
#include <rfl/json.hpp>

namespace Doodle
{

template <typename T> class DOO_API ISerializable
{

public:
    std::string Serialize()
    {
        return rfl::json::write(m_data);
    }

    void SerializeToFile(const std::string &path)
    {
        std::ofstream file(path);
        file << Serialize();
        file.close();
    }

    void Deserialize(const std::string &json)
    {
        SetData(rfl::json::read<T>(json).value());
    }

    void DeserializeFromFile(const std::string &path)
    {
        std::ifstream file(path);
        std::string json((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        Deserialize(json);
    }

    T GetData() const
    {
        return m_data;
    }

    void SetData(const T &data)
    {
        m_data = data;
        Apply();
    }

    virtual void Apply() {};

protected:
    T m_data;
};

} // namespace Doodle