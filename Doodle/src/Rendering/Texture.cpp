#include <array>
#include <cstddef>
#include <cstdint>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <stb_image.h>

#include "Buffer.h"
#include "Log.h"
#include "Renderer.h"
#include "Texture.h"
#include "Utils.h"

namespace Doodle
{

static size_t GetMemorySize(TextureFormat format, uint32_t width, uint32_t height)
{
    switch (format)
    {
    case TextureFormat::RGB8:
        return width * height * 3;
    case TextureFormat::RGB16F:
        return width * height * 3 * sizeof(uint16_t);
    case TextureFormat::RGB32F:
        return width * height * 3 * sizeof(float);
    case TextureFormat::RGBA8:
        return width * height * 4;
    case TextureFormat::RGBA16F:
        return width * height * 4 * sizeof(uint16_t);
    case TextureFormat::RGBA32F:
        return width * height * 4 * sizeof(float);
    case TextureFormat::SRGB8:
        return width * height * 3;
    case TextureFormat::SRGB8ALPHA8:
        return width * height * 4;
    case TextureFormat::DEPTH32F:
        return width * height * sizeof(float);
    case TextureFormat::DEPTH24STENCIL8:
        return width * height * sizeof(float);
    case TextureFormat::None:
        break;
    }
    return 0;
}

static GLenum GetInternalFormat(TextureFormat format)
{
    switch (format)
    {
    case TextureFormat::RGB8:
        return GL_RGB8;
    case TextureFormat::RGB16F:
        return GL_RGB16F;
    case TextureFormat::RGB32F:
        return GL_RGB32F;
    case TextureFormat::RGBA8:
        return GL_RGBA8;
    case TextureFormat::RGBA16F:
        return GL_RGBA16F;
    case TextureFormat::RGBA32F:
        return GL_RGBA32F;
    case TextureFormat::SRGB8:
        return GL_SRGB8;
    case TextureFormat::SRGB8ALPHA8:
        return GL_SRGB8_ALPHA8;
    case TextureFormat::DEPTH32F:
        return GL_DEPTH_COMPONENT32F;
    case TextureFormat::DEPTH24STENCIL8:
        return GL_DEPTH24_STENCIL8;
    default:
        DOO_CORE_ERROR("Unsupported texture format: {0}", TextureFormatToString(format));
        return GL_NONE;
    }
}

static std::tuple<GLenum, GLenum> GetFormatAndType(GLenum internalFormat)
{
    switch (internalFormat)
    {
    case GL_RGB8:
        return std::make_tuple(GL_RGB, GL_UNSIGNED_BYTE);
    case GL_RGBA8:
        return std::make_tuple(GL_RGBA, GL_UNSIGNED_BYTE);
    case GL_RGB16F:
        return std::make_tuple(GL_RGB, GL_FLOAT);
    case GL_RGBA16F:
        return std::make_tuple(GL_RGBA, GL_FLOAT);
    case GL_RGB32F:
        return std::make_tuple(GL_RGB, GL_FLOAT);
    case GL_RGBA32F:
        return std::make_tuple(GL_RGBA, GL_FLOAT);
    case GL_SRGB8:
        return std::make_tuple(GL_RGB, GL_UNSIGNED_BYTE);
    case GL_SRGB8_ALPHA8:
        return std::make_tuple(GL_RGBA, GL_UNSIGNED_BYTE);
    case GL_DEPTH_COMPONENT32F:
        return std::make_tuple(GL_DEPTH_COMPONENT, GL_FLOAT);
    case GL_DEPTH24_STENCIL8:
        return std::make_tuple(GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
    default:
        // Handle unsupported formats
        DOO_CORE_ERROR("Unsupported internal format: {0}", internalFormat);
        return std::make_tuple(GL_NONE, GL_NONE);
    }
}

static int CalculateMipMapCount(int width, int height)
{
    return static_cast<int>(glm::floor(glm::log2(static_cast<float>(glm::min(width, height))))) + 1;
}

class OpenGLTexture2D : public Texture2D
{
public:
    OpenGLTexture2D(const std::string &filepath, const TextureParams &params)
        : m_filepath(filepath), m_data(nullptr), m_params(params)
    {
        stbi_set_flip_vertically_on_load(true);
        m_hdr = stbi_is_hdr(m_filepath.c_str());

        int width, height, channels, desiredChannels = 0;
        switch (m_params.Format)
        {
        case TextureFormat::RGB8:
        case TextureFormat::RGB16F:
        case TextureFormat::RGB32F:
        case TextureFormat::SRGB8:
            desiredChannels = STBI_rgb;
            break;
        case TextureFormat::RGBA8:
        case TextureFormat::RGBA16F:
        case TextureFormat::RGBA32F:
        case TextureFormat::SRGB8ALPHA8:
            desiredChannels = STBI_rgb_alpha;
            break;
        case TextureFormat::DEPTH32F:
        case TextureFormat::DEPTH24STENCIL8:
            desiredChannels = STBI_grey;
            break;
        default:
            DOO_CORE_WARN("Texture format not specified");
            break;
        }
        if (m_hdr)
        {
            m_data = reinterpret_cast<std::byte *>(
                stbi_loadf(m_filepath.c_str(), &width, &height, &channels, desiredChannels));
        }
        else
        {
            m_data = reinterpret_cast<std::byte *>(
                stbi_load(m_filepath.c_str(), &width, &height, &channels, desiredChannels));
        }

        if (!m_data)
        {
            DOO_CORE_ERROR("Failed to load texture: {0}", m_filepath);
            return;
        }

        m_params.Width = width;
        m_params.Height = height;
        LoadTexture();
        Renderer::Submit([this]() { stbi_image_free(m_data); }); // Free the image data
    }

    OpenGLTexture2D(Buffer buffer, const TextureParams &params) : m_params(params)
    {
        auto size = GetMemorySize(m_params.Format, m_params.Width, m_params.Height);
        if (buffer)
        {
            m_data = Buffer::Copy(buffer, size);
        }
        else
        {
            Buffer buffer;
            buffer.Allocate(size);
            m_data = buffer;
        }
        LoadTexture();
    }

    ~OpenGLTexture2D()
    {
        Renderer::Submit([this]() {
            glMakeTextureHandleNonResidentARB(m_textureHandle);
            glDeleteTextures(1, &m_rendererId);
            DOO_CORE_TRACE("Texture <{0}> destroyed", m_rendererId);
        });
    }

    void Bind(uint32_t slot) override
    {
        m_binding = slot;
        Renderer::Submit([this]() {
            glBindTextureUnit(m_binding, m_rendererId);
            DOO_CORE_TRACE("Texture <{0}> bound to slot {1}", m_rendererId, m_binding);
        });
    }

    void Unbind() const override
    {
        Renderer::Submit([this]() {
            glBindTextureUnit(m_binding, 0);
            DOO_CORE_TRACE("Texture <{0}> unbound", m_rendererId);
        });
    }

    uint32_t GetWidth() const override
    {
        return m_params.Width;
    }
    uint32_t GetHeight() const override
    {
        return m_params.Height;
    }
    uint32_t GetRendererID() const override
    {
        return m_rendererId;
    }
    uint64_t GetTextureHandle() const override
    {
        return m_textureHandle;
    }
    uint32_t GetTarget() const override
    {
        return GL_TEXTURE_2D;
    }
    TextureFormat GetFormat() const override
    {
        return m_params.Format;
    }
    uint32_t GetMipLevelCount() const override
    {
        return CalculateMipMapCount(m_params.Width, m_params.Height);
    }

private:
    void LoadTexture()
    {
        auto width = m_params.Width;
        auto height = m_params.Height;
        Renderer::Submit([this, width, height]() {
            glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererId);

            GLenum internalFormat = GetInternalFormat(m_params.Format);
            GLenum format = std::get<0>(GetFormatAndType(internalFormat));
            GLenum type = std::get<1>(GetFormatAndType(internalFormat));
            uint32_t levels = GetMipLevelCount();
            glTextureStorage2D(m_rendererId, levels, internalFormat, width, height);
            if (m_data != nullptr)
                glTextureSubImage2D(m_rendererId, 0, 0, 0, width, height, format, type, m_data);
            glGenerateTextureMipmap(m_rendererId);

            // Set texture parameters
            switch (m_params.Wrap)
            {
            case TextureWrap::Repeat:
                glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_T, GL_REPEAT);
                break;
            case TextureWrap::MirroredRepeat:
                glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
                glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
                break;
            case TextureWrap::Clamp:
                glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                break;
            case TextureWrap::ClampToEdge:
                glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                break;
            case TextureWrap::None:
                DOO_CORE_WARN("Texture wrap mode not specified");
                break;
            }

            switch (m_params.Filter)
            {
            case TextureFilter::Nearest:
                glTextureParameteri(m_rendererId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTextureParameteri(m_rendererId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                break;
            case TextureFilter::Linear:
                glTextureParameteri(m_rendererId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTextureParameteri(m_rendererId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
            case TextureFilter::MipmapNearest:
                glTextureParameteri(m_rendererId, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                glTextureParameteri(m_rendererId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                break;
            case TextureFilter::MipmapLinear:
                glTextureParameteri(m_rendererId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTextureParameteri(m_rendererId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
            case TextureFilter::None:
                DOO_CORE_WARN("Texture filter mode not specified");
                break;
            }

            m_textureHandle = glGetTextureHandleARB(m_rendererId);
            glMakeTextureHandleResidentARB(m_textureHandle);
            DOO_CORE_TRACE("Texture <{0}> created: {1}\n{2}", m_rendererId, m_filepath, m_params.ToString());
        });
    }
    TextureParams m_params;
    uint32_t m_rendererId;
    uint64_t m_textureHandle;
    std::string m_filepath;
    std::byte *m_data = nullptr;
    bool m_hdr;
    uint32_t m_binding;
};

std::shared_ptr<Texture2D> Texture2D::Create(const std::string &filepath, const TextureParams &params)
{
    return std::make_shared<OpenGLTexture2D>(filepath, params);
}

std::shared_ptr<Texture2D> Texture2D::Create(Buffer buffer, const TextureParams &params)
{
    return std::make_shared<OpenGLTexture2D>(buffer, params);
}

std::shared_ptr<Texture2D> Texture2D::GetWhiteTexture()
{
    if (!s_WhiteTexture)
    {
        uint32_t data = 0xffffffff;
        Buffer buffer = Buffer::Copy(&data, sizeof(data));
        s_WhiteTexture = Texture2D::Create(buffer, TextureParams());
    }
    return s_WhiteTexture;
}

std::shared_ptr<Texture2D> Texture2D::GetBlackTexture()
{
    if (!s_BlackTexture)
    {
        uint32_t data = 0xff000000;
        Buffer buffer = Buffer::Copy(&data, sizeof(data));
        s_BlackTexture = Texture2D::Create(buffer, TextureParams());
    }
    return s_BlackTexture;
}

std::shared_ptr<Texture2D> Texture2D::GetDefaultNormalTexture()
{
    if (!s_DefaultNormalTexture)
    {
        uint32_t data = 0xffff8080;
        Buffer buffer = Buffer::Copy(&data, sizeof(data));
        s_DefaultNormalTexture = Texture2D::Create(buffer, TextureParams());
    }
    return s_DefaultNormalTexture;
}

std::shared_ptr<Texture2D> Texture2D::GetCheckerboardTexture()
{
    if (!s_CheckerboardTexture)
    {
        uint32_t checkerboard[4][4] = {{0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF, 0xFF000000},
                                       {0xFF000000, 0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF},
                                       {0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF, 0xFF000000},
                                       {0xFF000000, 0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF}};
        TextureParams params;
        params.Width = 4;
        params.Height = 4;
        params.Filter = TextureFilter::Nearest;
        Buffer buffer = Buffer::Copy(&checkerboard, sizeof(checkerboard));
        s_CheckerboardTexture = Texture2D::Create(buffer, params);
    }
    return s_CheckerboardTexture;
}

std::shared_ptr<Texture2D> Texture2D::s_WhiteTexture = nullptr;
std::shared_ptr<Texture2D> Texture2D::s_BlackTexture = nullptr;
std::shared_ptr<Texture2D> Texture2D::s_DefaultNormalTexture = nullptr;
std::shared_ptr<Texture2D> Texture2D::s_CheckerboardTexture = nullptr;

class OpenGLTextureCube : public TextureCube
{
public:
    OpenGLTextureCube(const std::array<std::string, 6> &facePaths, const TextureParams &params)
        : m_facePaths(facePaths), m_params(params)
    {
        stbi_set_flip_vertically_on_load(false);
        m_hdr = stbi_is_hdr(m_facePaths[0].c_str());

        int width, height, channels, desiredChannels = 0;
        switch (m_params.Format)
        {
        case TextureFormat::RGB8:
        case TextureFormat::RGB16F:
        case TextureFormat::RGB32F:
        case TextureFormat::SRGB8:
            desiredChannels = STBI_rgb;
            break;
        case TextureFormat::RGBA8:
        case TextureFormat::RGBA16F:
        case TextureFormat::RGBA32F:
        case TextureFormat::SRGB8ALPHA8:
            desiredChannels = STBI_rgb_alpha;
            break;
        case TextureFormat::DEPTH32F:
        case TextureFormat::DEPTH24STENCIL8:
            desiredChannels = STBI_grey;
            break;
        default:
            DOO_CORE_WARN("Texture format not specified");
            return;
        }

        // Load each face of the cube texture
        for (size_t i = 0; i < 6; ++i)
        {
            if (m_hdr)
            {
                m_faceData[i] = reinterpret_cast<std::byte *>(
                    stbi_loadf(m_facePaths[i].c_str(), &width, &height, &channels, desiredChannels));
            }
            else
            {
                m_faceData[i] = reinterpret_cast<std::byte *>(
                    stbi_load(m_facePaths[i].c_str(), &width, &height, &channels, desiredChannels));
            }

            GetMemorySize(m_params.Format, width, height);
            if (!m_faceData[i])
            {
                DOO_CORE_ERROR("Failed to load cube texture face: {0}", m_facePaths[i]);
                return;
            }
        }

        m_params.Width = width;
        m_params.Height = height;
        LoadTexture();
        Renderer::Submit([this]() {
            for (size_t i = 0; i < 6; ++i)
                stbi_image_free(m_faceData[i]);
        });
    }

    OpenGLTextureCube(const std::array<Buffer, 6> &faceBuffers, const TextureParams &params) : m_params(params)
    {
        auto size = GetMemorySize(m_params.Format, m_params.Width, m_params.Height);
        for (size_t i = 0; i < 6; ++i)
        {
            if (faceBuffers[i])
            {
                m_faceData[i] = Buffer::Copy(faceBuffers[i], size);
            }
        }
        LoadTexture();
    }

    OpenGLTextureCube(const TextureParams &params) : m_params(params)
    {
        LoadTexture();
    }

    ~OpenGLTextureCube()
    {
        Renderer::Submit([this]() {
            glMakeTextureHandleNonResidentARB(m_textureHandle);
            glDeleteTextures(1, &m_rendererId);
            DOO_CORE_TRACE("Cube Texture <{0}> destroyed", m_rendererId);
        });
    }

    void Bind(uint32_t slot) override
    {
        m_binding = slot;
        Renderer::Submit([this]() {
            glBindTextureUnit(m_binding, m_rendererId);
            DOO_CORE_TRACE("Cube Texture <{0}> bound to slot {1}", m_rendererId, m_binding);
        });
    }

    void Unbind() const override
    {
        Renderer::Submit([this]() {
            glBindTextureUnit(m_binding, 0);
            DOO_CORE_TRACE("Cube Texture <{0}> unbound", m_rendererId);
        });
    }

    uint32_t GetWidth() const override
    {
        return m_params.Width;
    }
    uint32_t GetHeight() const override
    {
        return m_params.Height;
    }
    uint32_t GetRendererID() const override
    {
        return m_rendererId;
    }
    uint64_t GetTextureHandle() const override
    {
        return m_textureHandle;
    }
    uint32_t GetTarget() const override
    {
        return GL_TEXTURE_CUBE_MAP;
    }
    TextureFormat GetFormat() const override
    {
        return m_params.Format;
    }
    uint32_t GetMipLevelCount() const override
    {
        return CalculateMipMapCount(m_params.Width, m_params.Height);
    }

private:
    void LoadTexture()
    {
        auto width = m_params.Width;
        auto height = m_params.Height;
        Renderer::Submit([this, width, height]() {
            glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_rendererId);

            GLenum internalFormat = GetInternalFormat(m_params.Format);
            GLenum format = std::get<0>(GetFormatAndType(internalFormat));
            GLenum type = std::get<1>(GetFormatAndType(internalFormat));
            uint32_t levels = GetMipLevelCount();
            glTextureStorage2D(m_rendererId, levels, internalFormat, width, height);

            // Upload each face to the cube map
            for (size_t i = 0; i < 6; ++i)
            {
                if (m_faceData[i] == nullptr)
                {
                    continue;
                }
                glTextureSubImage3D(m_rendererId, 0, 0, 0, static_cast<GLint>(i), width, height, 1, format, type,
                                    m_faceData[i]);
            }

            glGenerateTextureMipmap(m_rendererId);

            // Set texture parameters
            switch (m_params.Wrap)
            {
            case TextureWrap::Repeat:
                glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_R, GL_REPEAT);
                break;
            case TextureWrap::MirroredRepeat:
                glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
                glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
                glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
                break;
            case TextureWrap::Clamp:
                glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                break;
            case TextureWrap::ClampToEdge:
                glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                break;
            case TextureWrap::None:
                DOO_CORE_WARN("Texture wrap mode not specified");
                break;
            }

            switch (m_params.Filter)
            {
            case TextureFilter::Nearest:
                glTextureParameteri(m_rendererId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTextureParameteri(m_rendererId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                break;
            case TextureFilter::Linear:
                glTextureParameteri(m_rendererId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTextureParameteri(m_rendererId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
            case TextureFilter::MipmapNearest:
                glTextureParameteri(m_rendererId, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                glTextureParameteri(m_rendererId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                break;
            case TextureFilter::MipmapLinear:
                glTextureParameteri(m_rendererId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTextureParameteri(m_rendererId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
            case TextureFilter::None:
                DOO_CORE_WARN("Texture filter mode not specified");
                break;
            }

            m_textureHandle = glGetTextureHandleARB(m_rendererId);
            glMakeTextureHandleResidentARB(m_textureHandle);
            DOO_CORE_TRACE("Texture <{0}> created: {1}\n{2}", m_rendererId, m_facePaths[0], m_params.ToString());
        });
    }

    TextureParams m_params;
    uint32_t m_rendererId;
    uint64_t m_textureHandle;
    std::array<std::string, 6> m_facePaths;
    std::array<std::byte *, 6> m_faceData = {nullptr};
    uint32_t m_binding;
    bool m_hdr;
};

std::shared_ptr<TextureCube> TextureCube::Create(const std::array<std::string, 6> &facePaths,
                                                 const TextureParams &params)
{
    return std::make_shared<OpenGLTextureCube>(facePaths, params);
}

std::shared_ptr<TextureCube> TextureCube::Create(const std::array<Buffer, 6> &faceBuffers, const TextureParams &params)
{
    return std::make_shared<OpenGLTextureCube>(faceBuffers, params);
}

std::shared_ptr<TextureCube> TextureCube::Create(const TextureParams &params)
{
    return std::make_shared<OpenGLTextureCube>(params);
}

std::shared_ptr<TextureCube> TextureCube::GetWhiteTexture()
{
    if (!s_WhiteTexture)
    {
        uint32_t data = 0xffffffff;
        Buffer buffer(&data, sizeof(data));
        std::array<Buffer, 6> faceBuffers;
        faceBuffers[0] = faceBuffers[1] = faceBuffers[2] = faceBuffers[3] = faceBuffers[4] = faceBuffers[5] = buffer;
        s_WhiteTexture = TextureCube::Create(faceBuffers, TextureParams());
    }
    return s_WhiteTexture;
}

std::shared_ptr<TextureCube> TextureCube::GetBlackTexture()
{
    if (!s_BlackTexture)
    {
        uint32_t data = 0xff000000;
        Buffer buffer(&data, sizeof(data));
        std::array<Buffer, 6> faceBuffers;
        faceBuffers[0] = faceBuffers[1] = faceBuffers[2] = faceBuffers[3] = faceBuffers[4] = faceBuffers[5] = buffer;
        s_BlackTexture = TextureCube::Create(faceBuffers, TextureParams());
    }
    return s_BlackTexture;
}

std::shared_ptr<TextureCube> TextureCube::s_WhiteTexture = nullptr;
std::shared_ptr<TextureCube> TextureCube::s_BlackTexture = nullptr;

} // namespace Doodle
