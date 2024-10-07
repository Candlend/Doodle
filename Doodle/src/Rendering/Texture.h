#pragma once

#include "Buffer.h"
#include "Log.h"
#include "Renderer.h"
#include "UUID.h"
#include <vector>

namespace Doodle
{

enum class TextureFormat
{
    None = 0,
    RGB8,
    RGB16F,
    RGB32F,
    RGBA8,
    RGBA16F,
    RGBA32F,

    SRGB8,
    SRGB8ALPHA8,

    DEPTH32F,
    DEPTH24STENCIL8,

    // Defaults
    Depth = DEPTH24STENCIL8
};

enum class TextureWrap
{
    None = 0,
    Repeat = 1,
    MirroredRepeat = 2,
    Clamp = 3,
    ClampToEdge = 4,
};

enum class TextureFilter
{
    None = 0,
    Nearest = 1,
    Linear = 2,
    MipmapNearest = 3,
    MipmapLinear = 4,
};

struct TextureSpecification
{
    TextureFormat Format = TextureFormat::RGBA8;
    TextureWrap Wrap = TextureWrap::Repeat;
    TextureFilter Filter = TextureFilter::Linear;
    uint32_t Width = 1;
    uint32_t Height = 1;
    bool InvertColor = false; // only works for LDR textures
};

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
    static std::shared_ptr<Texture2D> Load(const std::filesystem::path &assetPath);
    static std::shared_ptr<Texture2D> Create(const std::filesystem::path &filepath,
                                             const TextureSpecification &spec = TextureSpecification());

    static std::shared_ptr<Texture2D> Create(Buffer buffer, const TextureSpecification &spec = TextureSpecification());

    static std::shared_ptr<Texture2D> GetWhiteTexture();

    static std::shared_ptr<Texture2D> GetBlackTexture();

    static std::shared_ptr<Texture2D> GetDefaultNormalTexture();

    static std::shared_ptr<Texture2D> GetCheckerboardTexture();

    std::filesystem::path GetPath() const;
};

class DOO_API TextureCube : public Texture
{
public:
    static std::shared_ptr<TextureCube> Load(const std::filesystem::path &assetPath);
    static std::shared_ptr<TextureCube> Create(const std::array<std::string, 6> &facePaths,
                                               const TextureSpecification &spec = TextureSpecification());
    static std::shared_ptr<TextureCube> Create(const std::array<Buffer, 6> &faceBuffers,
                                               const TextureSpecification &spec = TextureSpecification());

    static std::shared_ptr<TextureCube> Create(const TextureSpecification &spec = TextureSpecification());

    static std::shared_ptr<TextureCube> GetWhiteTexture();

    static std::shared_ptr<TextureCube> GetBlackTexture();

    std::array<std::string, 6> GetPath() const;
};


} // namespace Doodle
