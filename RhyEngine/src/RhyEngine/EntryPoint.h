#pragma once

#ifdef RHY_PLATFORM_WINDOWS

extern RhyEngine::Application* RhyEngine::CreateApplication();

int main(int argc, char** argv)
{
	auto app = RhyEngine::CreateApplication();
	app->Run();
	delete app;
}

#endif