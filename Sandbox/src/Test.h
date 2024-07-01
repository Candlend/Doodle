#pragma once

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
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
		};

		static unsigned int s_Indices[] = {
			0, 1, 2
		};

        Renderer::Clear(0.2f, 0.2f, 0.2f, 1.f);

        m_vbo = VBO::Create();
        m_vbo->SetData(s_Vertices, sizeof(s_Vertices));

        m_ibo = IBO::Create();
        m_ibo->SetData(s_Indices, sizeof(s_Indices));
        m_shader = Shader::Create("assets/shaders/shader.glsl");
    }

    void OnUpdate() override
    {
        Renderer::Clear(0.2f, 0.2f, 0.2f, 1.f);
        float timeValue = Application::Time::GetTime();
        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        m_shader->SetUniform4f("u_Color", 0.8f, greenValue, 0.2f, 1.0f);
        m_shader->Bind();
        m_vbo->Bind();
		m_ibo->Bind();
		Renderer::DrawIndexed(3);
    }

    void OnLayout() override
    {
        ImGui::ShowDemoWindow();

        ImGuiUtils::WindowScope scope("调试");
        if (scope.IsOpened())
        {
            ImGui::LabelText("时间", "%f", Application::Time::GetTime());
            ImGui::LabelText("帧率", "%f", Application::Time::GetFPS());
        }
    }


    void Deinitialize() override
    {
    }
private:
    std::shared_ptr<VBO> m_vbo;
    std::shared_ptr<IBO> m_ibo;
    std::shared_ptr<Shader> m_shader;
};