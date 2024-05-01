#pragma once

#include "RenderCommandQueue.h"
#include "RendererAPI.h"
#include "Singleton.h"

namespace Doodle
{

class DOO_API Renderer : public Singleton<Renderer>
{
public:
    template <typename... Args> static void Submit(void (*func)(Args...), Args... args)
    {
        using CommandType = RenderCommand<Args...>;
        auto command = new CommandType(func, args...);
        void *mem = Get().m_commandQueue.Allocate([](void *cmd) { static_cast<CommandType *>(cmd)->Execute(); },
                                                  sizeof(CommandType));
        new (mem) CommandType(*command);
    }

    static void Clear();
    static void Clear(float r, float g, float b, float a = 1.0f);
    static void SetClearColor(float r, float g, float b, float a);
    static void ClearMagenta();
    void Init();
    void WaitAndRender();

private:
    RenderCommandQueue m_commandQueue;
};

} // namespace Doodle
