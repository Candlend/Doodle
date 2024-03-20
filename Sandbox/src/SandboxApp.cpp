#include <RhyEngine.h>

class Sandbox : public RhyEngine::Application
{
public:
    Sandbox() {
        PushOverlay(new RhyEngine::ImGuiLayer());
    }

    ~Sandbox() override = default;
};

RhyEngine::Application *RhyEngine::CreateApplication()
{
    return new Sandbox();
}