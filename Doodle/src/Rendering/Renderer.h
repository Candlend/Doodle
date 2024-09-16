#pragma once

#include <typeinfo>

#include "ApplicationEvent.h"
#include "RenderCommandQueue.h"
#include "RendererAPI.h"
#include "Singleton.h"
#include "Texture.h"

namespace Doodle
{

class DOO_API Renderer : public Singleton<Renderer>
{
public:
    template <typename... Args>
    static void Submit(std::function<void(Args...)> func, Args... args)
    {
        using CommandType = RenderCommand<Args...>;
        auto command = new CommandType(func, args...);
        void *mem = Get()->m_commandQueue.Allocate([](void *cmd) { static_cast<CommandType *>(cmd)->Execute(); },
                                                   sizeof(CommandType));
        new (mem) CommandType(*command);
    }
    static void Submit(std::function<void()> func)
    {
        Submit<>(func);
    }
    static void Clear(float r, float g, float b, float a = 1.0f);
    static void Clear();
    static void SetClearColor(float r, float g, float b, float a = 1.0f);
    static void DrawIndexed(unsigned int count);
    static void UseWireframe(bool useWireframe);

    void Initialize();
    void Deinitialize();

private:
    void BeginFrame();
    void EndFrame();
    void WaitAndRender();

    RenderCommandQueue m_commandQueue;
};

} // namespace Doodle
