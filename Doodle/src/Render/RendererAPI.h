#pragma once

#include "Core.h"

namespace Doodle
{

class DOO_API RendererAPI
{
public:
    static void Clear(float r, float g, float b, float a);
    static void SetClearColor(float r, float g, float b, float a);
};

} // namespace Doodle