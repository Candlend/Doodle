#pragma once

#include "Renderer.h"

namespace Doodle
{

struct DOO_API ShaderUniform
{
};

struct DOO_API ShaderUniformCollection
{
};

class DOO_API Shader
{
public:
    static std::unique_ptr<Shader> Create(const std::string &filepath);
    virtual void Bind() = 0;
};

} // namespace Doodle