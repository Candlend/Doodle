#include <glad/glad.h>

#include "SceneEnvironment.h"
#include "SceneRenderer.h"
#include "Shader.h"
#include "Texture.h"

namespace Doodle
{

Environment Environment::Load(const std::string &filepath)
{
    const uint32_t CUBEMAP_SIZE = 2048;
    const uint32_t IRRADIANCE_MAP_SIZE = 32;

    TextureParams params;
    params.Width = CUBEMAP_SIZE;
    params.Height = CUBEMAP_SIZE;
    params.Format = TextureFormat::RGBA16F;
    params.Wrap = TextureWrap::ClampToEdge;

    std::shared_ptr<TextureCube> envUnfiltered = TextureCube::Create(params);
    static std::shared_ptr<Shader> s_EquirectangularConversionShader, s_EnvFilteringShader, s_EnvIrradianceShader;
    if (!s_EquirectangularConversionShader)
        s_EquirectangularConversionShader = Shader::Create("assets/shaders/equirectangularToCubeMap.glsl");

    TextureParams equirectParams;
    equirectParams.Format = TextureFormat::RGBA16F;
    std::shared_ptr<Texture2D> envEquirect = Texture2D::Create(filepath, equirectParams);
    DOO_CORE_ASSERT(envEquirect->GetFormat() == TextureFormat::RGBA16F, "Texture is not HDR!");

    s_EquirectangularConversionShader->Bind();
    envEquirect->Bind();
    Renderer::Submit([envUnfiltered, CUBEMAP_SIZE, envEquirect]() {
        glBindImageTexture(0, envUnfiltered->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
        glDispatchCompute(CUBEMAP_SIZE / 32, CUBEMAP_SIZE / 32, 6);
        glGenerateTextureMipmap(envUnfiltered->GetRendererID());
    });

    if (!s_EnvFilteringShader)
        s_EnvFilteringShader = Shader::Create("assets/shaders/environmentMipFilter.glsl");

    std::shared_ptr<TextureCube> envFiltered = TextureCube::Create(params);

    Renderer::Submit([envUnfiltered, envFiltered]() {
        glCopyImageSubData(envUnfiltered->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
                           envFiltered->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0, envFiltered->GetWidth(),
                           envFiltered->GetHeight(), 6);
    });

    s_EnvFilteringShader->Bind();
    envUnfiltered->Bind();

    Renderer::Submit([envUnfiltered, envFiltered, CUBEMAP_SIZE]() {
        const float DELTA_ROUGHNESS = 1.0f / glm::max((envFiltered->GetMipLevelCount() - 1.0f), 1.0f);
        for (int level = 1, size = CUBEMAP_SIZE / 2; level < envFiltered->GetMipLevelCount();
             level++, size /= 2) // <= ?
        {
            const GLuint NUM_GROUPS = glm::max(1, size / 32);
            glBindImageTexture(0, envFiltered->GetRendererID(), level, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
            glProgramUniform1f(s_EnvFilteringShader->GetRendererID(), 0, level * DELTA_ROUGHNESS);
            glDispatchCompute(NUM_GROUPS, NUM_GROUPS, 6);
        }
    });

    if (!s_EnvIrradianceShader)
        s_EnvIrradianceShader = Shader::Create("assets/shaders/environmentIrradiance.glsl");

    params.Width = IRRADIANCE_MAP_SIZE;
    params.Height = IRRADIANCE_MAP_SIZE;

    std::shared_ptr<TextureCube> irradianceMap = TextureCube::Create(params);
    s_EnvIrradianceShader->Bind();
    envFiltered->Bind();
    Renderer::Submit([irradianceMap]() {
        glBindImageTexture(0, irradianceMap->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
        glDispatchCompute(irradianceMap->GetWidth() / 32, irradianceMap->GetHeight() / 32, 6);
        glGenerateTextureMipmap(irradianceMap->GetRendererID());
    });

    return {envFiltered, irradianceMap};
}

} // namespace Doodle