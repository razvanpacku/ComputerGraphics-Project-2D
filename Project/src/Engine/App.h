#pragma once
#include <string>

#include "Window.h"
#include "Renderer/Renderer.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

//forward declaration
class Entity;

class App
{
public:
	static void Init(int32_t argc, char* argv[]);
	static App& Get(const std::string& name = "", uint16_t width = WINDOW_WIDTH, uint16_t height = WINDOW_HEIGHT);

	void Update();

	void SetEntityTracking(bool boidOrNest = false);

	void Run();
private:
	App(const std::string& name, uint16_t width, uint16_t height);
	~App();

	static int32_t argc;
	static char** argv;

	float lastFrameTime = 0.0f;
	float deltaTime = 0.0f;

	bool isTrackingEntity = false;
	Entity* trackedEntity = nullptr;
	void UpdateEntityTracking(Entity* entity);

	Window *window;
	Renderer *renderer;
};

