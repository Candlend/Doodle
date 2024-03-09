#include <RhyEngine.h>

class Sandbox : public RhyEngine::Application
{
public:
    Sandbox() = default;

    ~Sandbox() override = default;
};

RhyEngine::Application *RhyEngine::CreateApplication()
{
    return new Sandbox();
}