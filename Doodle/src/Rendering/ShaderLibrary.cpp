#include "pch.h"
#include <filesystem>

#include "ShaderLibrary.h"

namespace Doodle
{

void ShaderLibrary::LoadShadersFromDirectory(const std::string &directory)
{
    DOO_CORE_TRACE("ShaderLibrary: Loading shaders from directory <{0}>", directory);
    for (const auto &entry : std::filesystem::directory_iterator(directory))
    {
        if (entry.is_regular_file())
        {
            const auto &path = entry.path();
            auto filename = path.filename().string();
            auto extension = path.extension().string();
            auto name = filename.substr(0, filename.size() - extension.size());
            if (extension == ".glsl")
                LoadShader(name, path.string());
        }
    }
}

void ShaderLibrary::AddShader(const std::string &name, std::shared_ptr<Shader> shader)
{
    if (Exists(name))
    {
        DOO_CORE_WARN("Shader <{0}> already exists!", name);
        return;
    }
    m_shaders[name] = shader;
}

std::shared_ptr<Shader> ShaderLibrary::LoadShader(const std::string &name, const std::string &filepath)
{
    auto shader = Shader::Create(filepath);
    AddShader(name, shader);
    DOO_CORE_TRACE("ShaderLibrary: Loaded shader <{0}> from <{1}>", name, filepath);
    return shader;
}

std::shared_ptr<Shader> ShaderLibrary::GetShader(const std::string &name)
{
    DOO_CORE_ASSERT(Exists(name), "Shader <{0}> not found!", name);
    return m_shaders[name];
}

bool ShaderLibrary::Exists(const std::string &name) const
{
    return m_shaders.find(name) != m_shaders.end();
}

} // namespace Doodle
