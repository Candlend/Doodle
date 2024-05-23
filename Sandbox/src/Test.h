#pragma once

#include <Doodle.h>

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

        m_vb = std::unique_ptr<VertexBuffer>(VertexBuffer::Create());
		m_vb->SetData(s_Vertices, sizeof(s_Vertices));

		m_ib = std::unique_ptr<IndexBuffer>(IndexBuffer::Create());
		m_ib->SetData(s_Indices, sizeof(s_Indices));
        m_shader = Shader::Create("assets/shaders/shader.glsl");
    }

    void OnUpdate() override
    {
        Renderer::Clear(0.2f, 0.2f, 0.2f, 1.f);
        m_shader->SetUniform4f("u_Color", 0.8f, 0.3f, 0.2f, 1.0f);
        m_shader->Bind();
        m_vb->Bind();
		m_ib->Bind();
		Renderer::DrawIndexed(3);
    }

    void OnLayout() override
    {
        ImGui::ShowDemoWindow();

        ImGuiUtils::WindowScope scope("测试");
        if (scope.IsOpened())
        {
            ImGui::Text("%s 你好，世界！", ICON_FA_WINDOW);
            ImGuiUtils::ChildWindowScope childWindow("childWindow", ImVec2(200, 200), ImGuiChildFlags_Border);
            if (childWindow.IsOpened())
            {
                ImGui::Text("Hello, child window!");
            }
        }
    }


    void Deinitialize() override
    {
    }
private:
	std::unique_ptr<VertexBuffer> m_vb;
	std::unique_ptr<IndexBuffer> m_ib;
	std::unique_ptr<Shader> m_shader;
};