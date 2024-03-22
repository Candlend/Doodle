#include <RhyEngine.h>
#include <memory>

class Sandbox : public RhyEngine::Application
{
public:
    Sandbox() {
        PushOverlay(new RhyEngine::ImGuiLayer());
    }

    ~Sandbox() override = default;
};

RhyEngine::Application* RhyEngine::CreateApplication()
{
	return new Sandbox();
}