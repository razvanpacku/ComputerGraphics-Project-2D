#include "./App.h"

int App::argc = 0;
char** App::argv = nullptr;

App::App(const std::string& name, uint16_t width, uint16_t height)
{
	window = new Window(width, height, name, argc, argv);
	renderer = new Renderer();
}

App::~App()
{
	delete window;
	delete renderer;
}

void App::Init(int argc, char* argv[])
{
	App::argc = argc;
	App::argv = argv;
}

App& App::Get(const std::string& name, uint16_t width, uint16_t height)
{
	static App* instance = nullptr;
	if (instance == nullptr)
	{
		instance = new App(name, width, height);
	}
	return *instance;
}

void App::Run()
{
	glutMainLoop();
}

