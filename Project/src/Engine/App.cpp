#include "./App.h"

#include "Renderer/TextureManager.h"
#include "../Boid/Boid.h"
#include "../Boid/BoidScene.h"

#include <cmath>
#include <ctime>

int32_t App::argc = 0;
char** App::argv = nullptr;

App::App(const std::string& name, uint16_t width, uint16_t height)
{
	srand(static_cast<unsigned int>(time(0)));

	window = new Window(width, height, name, argc, argv);
	renderer = new Renderer(this);

	// Set up camera
	Camera* camera = new Camera();
	renderer->SetCamera(camera);

	//load textures
	auto tex1 = TextureManager::Load("textures/dev.png");

	// Define a square mesh
	std::vector<glm::vec2> vertices = { {-0.5f,-0.5f}, {0.5f,-0.5f}, {0.5f,0.5f}, {-0.5f,0.5f} };
	std::vector<glm::vec3> colors = { {1,0,0}, {0,1,0}, {0,0,1}, {1,1,0} };
	std::vector<glm::vec2> texCoords = { {0,0}, {1,0}, {1,1}, {0,1} };
	std::vector<uint32_t> indices = { 0,1,2, 2,3,0 };

	auto squareMesh = renderer->AddMesh("square", std::make_shared<Mesh>(vertices, colors, texCoords, indices));

	Scene* scene = new BoidScene(renderer);
	auto boidMesh = renderer->AddMesh("boid", BoidScene::CreateBoidMesh());
	Boid::InitSharedResources(boidMesh, tex1);
	dynamic_cast<BoidScene*>(scene)->AddBackground(squareMesh, tex1);
	dynamic_cast<BoidScene*>(scene)->InitBoids(100);

	renderer->SetScene(scene);
}

App::~App()
{
	delete window;
	delete renderer;
}

void App::Init(int32_t argc, char* argv[])
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

void App::SetEntityTracking()
{
	isTrackingEntity = !isTrackingEntity;

	//special behaviour for boidscene
	if (isTrackingEntity){
		trackedEntity = dynamic_cast<BoidScene*>(renderer->GetScene())->GetRandomBoid();
		renderer->GetCamera()->targetEntity = trackedEntity;
	}

	if (isTrackingEntity && trackedEntity) {
		renderer->GetCamera()->hasTarget = true;
	}
	else {
		renderer->GetCamera()->hasTarget = false;
	}
}

void App::UpdateEntityTracking(Entity* entity)
{
	trackedEntity = entity;
	renderer->GetCamera()->targetEntity = entity;
}

void App::Run()
{
	glutMainLoop();
}

void App::Update()
{
	float currentFrameTime = static_cast<float>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
	deltaTime = currentFrameTime - lastFrameTime;
	lastFrameTime = currentFrameTime;

	if (renderer && renderer->GetScene())
		renderer->GetScene()->Update(deltaTime);
}

