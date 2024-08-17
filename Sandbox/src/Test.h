#pragma once

#include "TextureParams.h"
#include "glm/fwd.hpp"
#include "imgui.h"
#include <Doodle.h>
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
        vbo->PushElement<float, 3>("a_Position", false);
        vbo->PushElement<float, 3>("a_Color", false);
        vbo->PushElement<float, 2>("a_TexCoord", false);

        auto ibo = IndexBuffer::Create(s_Indices, sizeof(s_Indices));

        m_vao = VAO::Create();
        m_vao->AddVertexBuffer(vbo);
        m_vao->SetIndexBuffer(ibo);
        m_shader = Shader::Create("assets/shaders/shader.glsl");
        TextureParams params;
        params.Format = TextureFormat::RGB8;
        m_texture2D = Texture2D::Create("assets/icons/icon_large.png", params);
        uint32_t textureUnit = m_texture2D->Bind();
        m_shader->SetUniform1i("u_Texture", textureUnit);
    }

    void OnUpdate() override
    {
        Renderer::UseWireframe(m_useWireframe);
        Renderer::Clear();
        float timeValue = Application::Time::GetTime();
        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        m_shader->SetUniform4f("u_Color", 0.8f, greenValue, 0.2f, 1.0f);
        m_shader->SetUniform1i("u_Debug", m_debug);
        m_shader->Bind();
        m_vao->Render();
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
            ImGui::Checkbox("调试模式", &m_debug);
        }
    }

    void Deinitialize() override
    {
    }

private:
    bool m_debug = false;
    bool m_useWireframe = false;
    std::shared_ptr<VAO> m_vao;
    std::shared_ptr<Shader> m_shader;
    std::shared_ptr<Texture2D> m_texture2D;
};