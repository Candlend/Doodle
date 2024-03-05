#include <RhyEngine.h>

class Sandbox : public RhyEngine::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{

	}

};

RhyEngine::Application* RhyEngine::CreateApplication()
{
	return new Sandbox();
}