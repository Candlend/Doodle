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
    virtual uint32_t Bind(uint32_t slot) const = 0;
    uint32_t Bind() const
    {
        return Bind(0);
    }
    virtual void Unbind() const = 0;
    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;
    virtual uint32_t GetRendererID() const = 0;
};

class DOO_API Texture2D : public Texture
{
public:
    static std::shared_ptr<Texture2D> Create(const std::string &filepath,
                                             const TextureParams &params = TextureParams());
};

} // namespace Doodle
