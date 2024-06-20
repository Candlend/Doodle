#pragma once

#include <typeinfo>

#include "RenderCommandQueue.h"
#include "RendererAPI.h"
#include "Singleton.h"

namespace Doodle
{

class DOO_API GraphicsContext
{
public:
    virtual void Initialize() = 0;
    virtual void SwapBuffers() = 0;

    static GraphicsContext *Create(void *window);
};

} // namespace Doodle
