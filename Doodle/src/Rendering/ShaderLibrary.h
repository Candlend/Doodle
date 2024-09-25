#pragma once

#include "pch.h"

#include "Shader.h"

namespace Doodle
{

class DOO_API ShaderLibrary : public Singleton<ShaderLibrary>
{
public:
    void LoadShadersFromDirectory(const std::string &directory);
    void AddShader(const std::string &name, std::shared_ptr<Shader> shader);
    std::shared_ptr<Shader> LoadShader(const std::string &name, const std::filesystem::path &filepath);
    std::shared_ptr<Shader> GetShader(const std::string &name);
    bool Exists(const std::string &name) const;

    void Clear()
    {
        Get();
    }

private:
    std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
};

} // namespace Doodle