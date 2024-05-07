#pragma once

#include "Core.h"

namespace Doodle
{

using RendererID = unsigned int;

class DOO_API RendererAPI
{
public:
    static void Initialize();

    static void Clear(float r, float g, float b, float a);
    static void SetClearColor(float r, float g, float b, float a);

    static void DrawIndexed(unsigned int count);
};

} // namespace Doodle