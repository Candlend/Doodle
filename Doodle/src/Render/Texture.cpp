#include <cstdint>
#include <glad/glad.h>
#include <stb_image.h>

#include "Log.h"
#include "Renderer.h"
#include "Texture.h"
#include "TextureParams.h"

namespace Doodle
{

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
    int levels = 1;
    while ((width | height) >> levels)
    {
        levels++;
    }
    return levels;
}

class OpenGLTexture2D : public Texture2D
{
public:
    explicit OpenGLTexture2D(const std::string &filepath, const TextureParams &params)
        : m_filepath(filepath), m_width(0), m_height(0), m_data(nullptr), m_params(params)
    {
        LoadTexture();
    }

    ~OpenGLTexture2D()
    {
        Renderer::Submit([this]() {
            glDeleteTextures(1, &m_rendererId);
            DOO_CORE_TRACE("Texture <{0}> destroyed", m_rendererId);
        });
    }

    uint32_t Bind(uint32_t slot) const override
    {
        Renderer::Submit([this, slot]() {
            glBindTextureUnit(slot, m_rendererId);
            DOO_CORE_TRACE("Texture <{0}> bound to slot {1}", m_rendererId, slot);
        });
        return GL_TEXTURE0 + slot;
    }

    void Unbind() const override
    {
        Renderer::Submit([this]() {
            glBindTexture(GL_TEXTURE_2D, 0);
            DOO_CORE_TRACE("Texture <{0}> unbound", m_rendererId);
        });
    }

    uint32_t GetWidth() const override
    {
        return m_width;
    }
    uint32_t GetHeight() const override
    {
        return m_height;
    }
    uint32_t GetRendererID() const override
    {
        return m_rendererId;
    }

private:
    void LoadTexture()
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

        m_data =
            reinterpret_cast<std::byte *>(stbi_load(m_filepath.c_str(), &width, &height, &channels, desiredChannels));

        if (!m_data)
        {
            DOO_CORE_ERROR("Failed to load texture: {0}", m_filepath);
            return;
        }

        m_width = width;
        m_height = height;

        Renderer::Submit([this, width, height, channels]() {
            glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererId);

            GLenum internalFormat = GetInternalFormat(m_params.Format);
            GLenum format = std::get<0>(GetFormatAndType(internalFormat));
            GLenum type = std::get<1>(GetFormatAndType(internalFormat));

            glTextureStorage2D(m_rendererId, 1, internalFormat, width, height);
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
            stbi_image_free(m_data);
            DOO_CORE_TRACE("Texture <{0}> created: {1} ({2}x{3}x{4}) HDR={5}\n{6}", m_rendererId, m_filepath, width,
                           height, channels, m_hdr, m_params.ToString());
        });
    }

    TextureParams m_params;
    uint32_t m_rendererId;
    std::string m_filepath;
    uint32_t m_width, m_height;
    std::byte *m_data;
    bool m_hdr;
    bool m_sRgb;
};

std::shared_ptr<Texture2D> Texture2D::Create(const std::string &filepath, const TextureParams &params)
{
    return std::make_shared<OpenGLTexture2D>(filepath, params);
}

} // namespace Doodle
