#pragma once

#include "Material.h"
#include "VertexBuffer.h"
#include <Doodle.h>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

using namespace Doodle;

class Test : public RegisterModule<Test>
{
public:
    void Initialize() override
    {
        static float s_Vertices[] = {
            //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
            0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // 右上
            0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // 右下
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // 左下
            -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // 左上
        };
        static unsigned int s_Indices[] = {0, 1, 2, 2, 3, 0};

        Renderer::SetClearColor(0.2f, 0.2f, 0.2f, 1.f);

        auto vbo = VertexBuffer::Create(s_Vertices, sizeof(s_Vertices));
        vbo->PushElement("a_Position", VertexDataType::Vec3);
        vbo->PushElement("a_Color", VertexDataType::Vec3);
        vbo->PushElement("a_TexCoord", VertexDataType::Vec2);

        auto ibo = IndexBuffer::Create(s_Indices, sizeof(s_Indices));

        m_vao = VAO::Create();
        m_vao->AddVertexBuffer(vbo);
        m_vao->SetIndexBuffer(ibo);
        ShaderLibrary::Get().LoadShader("Test", "assets/shaders/shader.glsl");
        TextureParams params;
        params.Format = TextureFormat::SRGB8ALPHA8;
        m_texture2D = Texture2D::Create("assets/icons/icon_large.png", params);

        static glm::mat4 s_Model(1.0f);
        s_Model = glm::rotate(s_Model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        static glm::mat4 s_View(1.0f);
        s_View = glm::translate(s_View, glm::vec3(0.0f, 0.0f, -3.0f));
        static glm::mat4 s_Projection(1.0f);
        s_Projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        auto material = Material::Create("Test");
        material->SetUniform4f("u_Color", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

        m_materialInstance = MaterialInstance::Create(material);
        m_materialInstance->SetUniformTexture("u_Texture", m_texture2D);
        m_materialInstance->SetUniformMatrix4f("u_Model", s_Model);
        m_materialInstance->SetUniformMatrix4f("u_View", s_View);
        m_materialInstance->SetUniformMatrix4f("u_Projection", s_Projection);

        m_activeScene = Scene::Create();
        auto entity = m_activeScene->CreateEntity();
        entity.AddComponent<VAOComponent>(m_vao);
        entity.AddComponent<MaterialComponent>(m_materialInstance);
    }

    void OnUpdate() override
    {
        Renderer::UseWireframe(m_useWireframe);
        Renderer::Clear();
        float timeValue = Application::Time::GetTime();
        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        m_materialInstance->SetUniform4f("u_Color", glm::vec4(0.8f, greenValue, 0.2f, 1.0f));
        m_materialInstance->SetUniform1i("u_Debug", m_useWireframe);

        m_activeScene->Render();
    }

    void OnLayout() override
    {
        ImGui::ShowDemoWindow();

        ImGuiUtils::WindowScope scope("调试");
        if (scope.IsOpened())
        {
            ImGui::LabelText("时间", "%f", Application::Time::GetTime());
            ImGui::LabelText("帧率", "%f", Application::Time::GetFPS());
            ImGui::Checkbox("线框模式", &m_useWireframe);
        }
    }

    void Deinitialize() override
    {
    }

private:
    bool m_useWireframe = false;
    std::shared_ptr<Scene> m_activeScene;
    std::shared_ptr<MaterialInstance> m_materialInstance;
    std::shared_ptr<Shader> m_shader;
    std::shared_ptr<VAO> m_vao;
    std::shared_ptr<Texture2D> m_texture2D;
};