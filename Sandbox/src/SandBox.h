#pragma once

#include <Doodle.h>
#include <memory>


using namespace Doodle;

class Sandbox : public Application
{
public:
    void Initialize() override
    {
        Application::Initialize();
        ActivateImGuiContext();
        DOO_INFO("Sandbox initialized");

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

    }

    void Deinitialize() override
    {
        Application::Deinitialize();
        DOO_INFO("Sandbox deinitialized");
    }

    void OnUpdate() override
    {
        Application::OnUpdate();
    }


    void OnRender() override
    {
        Application::OnRender();
        Renderer::Clear(0.2f, 0.2f, 0.2f, 1.f);

        m_vb->Bind();
		m_ib->Bind();
		Renderer::DrawIndexed(3);
    }

    void OnLayout() override
    {
        Application::OnLayout();
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

private:
	std::unique_ptr<VertexBuffer> m_vb;
	std::unique_ptr<IndexBuffer> m_ib;
};
