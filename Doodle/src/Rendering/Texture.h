#pragma once

#include "Buffer.h"
#include "Log.h"
#include "Renderer.h"
#include "TextureParams.h"
#include "UUID.h"
#include <vector>

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
    virtual uint64_t GetTextureHandle() const = 0;
    virtual uint32_t GetTarget() const = 0;
    virtual TextureFormat GetFormat() const = 0;
    virtual uint32_t GetMipLevelCount() const = 0;
};

class DOO_API Texture2D : public Texture
{
public:
    static std::shared_ptr<Texture2D> Create(const std::string &filepath,
                                             const TextureParams &params = TextureParams());

    static std::shared_ptr<Texture2D> Create(Buffer buffer, const TextureParams &params = TextureParams());

    static std::shared_ptr<Texture2D> GetWhiteTexture();

    static std::shared_ptr<Texture2D> GetBlackTexture();

    static std::shared_ptr<Texture2D> GetDefaultNormalTexture();

    static std::shared_ptr<Texture2D> GetCheckerboardTexture();

    std::string GetPath() const;
};

class DOO_API TextureCube : public Texture
{
public:
    static std::shared_ptr<TextureCube> Create(const std::array<std::string, 6> &facePaths,
                                               const TextureParams &params = TextureParams());
    static std::shared_ptr<TextureCube> Create(const std::array<Buffer, 6> &faceBuffers,
                                               const TextureParams &params = TextureParams());

    static std::shared_ptr<TextureCube> Create(const TextureParams &params = TextureParams());

    static std::shared_ptr<TextureCube> GetWhiteTexture();

    static std::shared_ptr<TextureCube> GetBlackTexture();

    std::array<std::string, 6> GetPath() const;
};


} // namespace Doodle
