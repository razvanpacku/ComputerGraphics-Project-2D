#include "src/Engine/App.h"

int main(int argc, char* argv[])
{
	int32_t argc0 = static_cast<int32_t>(argc);
	App::Init(argc0, argv);
	App* app = &App::Get("Boids");

	app->Run();
}

