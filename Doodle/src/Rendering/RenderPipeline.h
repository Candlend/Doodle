#pragma once

#include "Framebuffer.h"
#include "SceneRenderer.h"
#include "pch.h"

#include "Light.h"
#include "Singleton.h"
#include "UniformBuffer.h"
#include <unordered_map>

namespace Doodle
{

class RenderPass;
class RenderPassSpecification;
class DOO_API RenderPipeline : public Singleton<RenderPipeline>
{
public:
    RenderPipeline();

    void RegisterRenderPasses();

    void AddRenderPass(const std::string &name, std::shared_ptr<RenderPass> renderPass);

    template <typename T> void CreateRenderPass(const std::string &name, const RenderPassSpecification &specification);

    void RemoveRenderPass(const std::string &name);

    std::shared_ptr<RenderPass> GetRenderPass(const std::string &name);

    void BeginScene(Scene *scene);

    void EndScene();

    void Execute();

    void SetUniformBuffer(const std::string &name, std::shared_ptr<UniformBuffer> uniformBuffer);
    void SetFrameBuffer(const std::string &name, std::shared_ptr<FrameBuffer> frameBuffer);
    void SetUniform1f(const std::string &name, float value);
    void SetUniform2f(const std::string &name, glm::vec2 value);
    void SetUniform3f(const std::string &name, glm::vec3 value);
    void SetUniform4f(const std::string &name, glm::vec4 value);
    void SetUniform1i(const std::string &name, int value);
    void SetUniform2i(const std::string &name, glm::ivec2 value);
    void SetUniform3i(const std::string &name, glm::ivec3 value);
    void SetUniform4i(const std::string &name, glm::ivec4 value);
    void SetUniformMatrix3f(const std::string &name, glm::mat3 value);
    void SetUniformMatrix4f(const std::string &name, glm::mat4 value);
    void SetUniformTexture(const std::string &name, std::shared_ptr<Texture> value);

    std::shared_ptr<UniformBuffer> GetUniformBuffer(const std::string &name);
    std::shared_ptr<FrameBuffer> GetFrameBuffer(const std::string &name);
    float GetUniform1f(const std::string &name);
    glm::vec2 GetUniform2f(const std::string &name);
    glm::vec3 GetUniform3f(const std::string &name);
    glm::vec4 GetUniform4f(const std::string &name);
    int GetUniform1i(const std::string &name);
    glm::ivec2 GetUniform2i(const std::string &name);
    glm::ivec3 GetUniform3i(const std::string &name);
    glm::ivec4 GetUniform4i(const std::string &name);
    glm::mat3 GetUniformMatrix3f(const std::string &name);
    glm::mat4 GetUniformMatrix4f(const std::string &name);
    std::shared_ptr<Texture> GetUniformTexture(const std::string &name);

private:
    Scene *m_scene;
    std::unordered_map<std::string, std::shared_ptr<RenderPass>> m_renderPasses;
    std::unordered_map<std::string, std::shared_ptr<UniformBuffer>> m_uniformBuffers;
    std::unordered_map<std::string, std::shared_ptr<FrameBuffer>> m_frameBuffers;
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
    std::unordered_map<std::string, float> m_uniforms1f;
    std::unordered_map<std::string, glm::vec2> m_uniforms2f;
    std::unordered_map<std::string, glm::vec3> m_uniforms3f;
    std::unordered_map<std::string, glm::vec4> m_uniforms4f;
    std::unordered_map<std::string, float> m_uniforms1i;
    std::unordered_map<std::string, glm::ivec2> m_uniforms2i;
    std::unordered_map<std::string, glm::ivec3> m_uniforms3i;
    std::unordered_map<std::string, glm::ivec4> m_uniforms4i;
    std::unordered_map<std::string, glm::mat3> m_uniforms3m;
    std::unordered_map<std::string, glm::mat4> m_uniforms4m;
};

} // namespace Doodle