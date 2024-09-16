#pragma once

#include "pch.h"

#include "Texture.h"

namespace Doodle
{

struct DOO_API Environment
{
    std::shared_ptr<TextureCube> RadianceMap;
    std::shared_ptr<TextureCube> IrradianceMap;

    static Environment Load(const std::string &filepath);
};

} // namespace Doodle
