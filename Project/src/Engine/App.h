#pragma once
#include <string>

#include "Window.h"
#include "Renderer/Renderer.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

class App
{
public:
	static void Init(int argc, char* argv[]);
	static App& Get(const std::string& name = "", uint16_t width = WINDOW_WIDTH, uint16_t height = WINDOW_HEIGHT);

	void Run();
private:
	App(const std::string& name, uint16_t width, uint16_t height);
	~App();

	static int argc;
	static char** argv;

	Window *window;
	Renderer *renderer;
};

