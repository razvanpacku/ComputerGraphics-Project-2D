#include "src/Engine/App.h"

int main(int argc, char* argv[])
{
	App::Init(argc, argv);
	App* app = &App::Get("Boids");

	app->Run();
}

