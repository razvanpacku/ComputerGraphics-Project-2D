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

	//Load default texture
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

	// Preload cloud textures (textures/cloud01.png .. textures/cloud10.png)
	std::vector<std::shared_ptr<Texture>> cloudTextures;
	cloudTextures.reserve(10);
	for (int i = 1; i <= 10; ++i) {
		char path[128];
		std::snprintf(path, sizeof(path), "textures/cloud%02d.png", i);
		cloudTextures.push_back(TextureManager::Load(path));
	}

	// Initialize clouds first so they are rendered before boids (boids will be drawn on top)
	if (auto boidScene = dynamic_cast<BoidScene*>(scene)) {
		boidScene->InitClouds(15);
	}

	// Create boids after clouds so they render above them and are visible immediately
	dynamic_cast<BoidScene*>(scene)->InitBoids(100);

	renderer->SetScene(scene);

	// Prevent a large first-frame deltaTime by initializing lastFrameTime to the current time
	lastFrameTime = static_cast<float>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
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

	//Special behaviour for boidscene
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

	// Clamp deltaTime to avoid huge jumps on the first frame or after stalls
	if (deltaTime > 0.05f) deltaTime = 0.05f;

	if (renderer && renderer->GetScene())
		renderer->GetScene()->Update(deltaTime);
}

