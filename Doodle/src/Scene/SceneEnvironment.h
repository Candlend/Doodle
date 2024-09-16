#pragma once

#include "pch.h"

#include "Texture.h"

namespace Doodle
{

struct Environment
{
    std::string FilePath;
    std::shared_ptr<TextureCube> RadianceMap;
    std::shared_ptr<TextureCube> IrradianceMap;

    static Environment Load(const std::string &filepath);
};

} // namespace Doodle
