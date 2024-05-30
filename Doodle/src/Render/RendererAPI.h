#pragma once

#include "pch.h"

namespace Doodle
{

struct RenderAPICapabilities
{
    std::string Vendor;
    std::string Renderer;
    std::string Version;

    int MaxSamples;
    float MaxAnisotropy;
};

class DOO_API RendererAPI
{
public:
    static void Initialize();

    static void Clear(float r, float g, float b, float a);
    static void SetClearColor(float r, float g, float b, float a);

    static void DrawIndexed(unsigned int count, bool depthTest = true);

    static RenderAPICapabilities &GetCapabilities()
    {
        static RenderAPICapabilities s_Capabilities;
        return s_Capabilities;
    }
};

} // namespace Doodle