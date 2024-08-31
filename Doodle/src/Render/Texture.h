#pragma once

#include "Log.h"
#include "Renderer.h"
#include "TextureParams.h"
#include <cstdint>

namespace Doodle
{

class DOO_API Texture
{
public:
    virtual ~Texture()
    {
    }
    virtual void Bind(uint32_t slot) = 0;
    void Bind()
    {
        Bind(0);
    }
    virtual void Unbind() const = 0;
    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;
    virtual uint32_t GetRendererID() const = 0;
    virtual uint32_t GetBinding() const = 0;
    virtual uint32_t GetTarget() const = 0;
};

class DOO_API Texture2D : public Texture
{
public:
    static std::shared_ptr<Texture2D> Create(const std::string &filepath,
                                             const TextureParams &params = TextureParams());
};

class DOO_API TextureCube : public Texture
{
public:
    static std::shared_ptr<TextureCube> Create(const std::array<std::string, 6>& facePaths,
                                                const TextureParams& params = TextureParams());
};


} // namespace Doodle
