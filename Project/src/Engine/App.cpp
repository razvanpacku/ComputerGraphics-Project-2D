#include "./App.h"

#include "Renderer/TextureManager.h"

#include <cmath>

int App::argc = 0;
char** App::argv = nullptr;

App::App(const std::string& name, uint16_t width, uint16_t height)
{
	window = new Window(width, height, name, argc, argv);
	renderer = new Renderer(this);

	// Set up camera
	Camera* camera = new Camera();
	renderer->SetCamera(camera);

	//load textures
	auto tex1 = TextureManager::Load("textures/dev.png");

	// app logic
	Scene* scene = new Scene(renderer);
	scene->backgroundColor = glm::vec3(0.1f, 0.1f, 0.1f);

	// Define a square mesh
	std::vector<glm::vec2> vertices = { {-0.5f,-0.5f}, {0.5f,-0.5f}, {0.5f,0.5f}, {-0.5f,0.5f} };
	std::vector<glm::vec3> colors = { {1,0,0}, {0,1,0}, {0,0,1}, {1,1,0} };
	std::vector<glm::vec2> texCoords = { {0,0}, {1,0}, {1,1}, {0,1} };
	std::vector<unsigned int> indices = { 0,1,2, 2,3,0 };

	auto squareMesh = renderer->AddMesh("square", std::make_shared<Mesh>(vertices, colors, texCoords, indices));

	auto ent1 = scene->AddEntity(std::make_unique<Entity>(squareMesh, glm::vec2(0.0f), glm::vec2(0.5f), 45.0f, tex1));
	auto ent2 = scene->AddEntity(std::make_unique<Entity>(squareMesh, glm::vec2(-0.5f, -0.5f), glm::vec2(0.3f), 30.0f));
	ent2->useTexture = false;

	UpdateEntityTracking(ent2);

	scene->SetUpdateFunction([ent1, ent2](float deltaTime) {
		ent1->rotation -= 30.0f * deltaTime;
		ent2->rotation -= 30.0f * deltaTime;

		static float timeAccumulator = 0.0f;
		timeAccumulator += deltaTime;

		ent1->position.x = sinf(timeAccumulator / 1.85f) * 4.5f;
		ent1->position.y = cosf(timeAccumulator / 1.85f) * 4.5f;
		});

	renderer->SetScene(scene);
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

void App::SetEntityTracking()
{
	isTrackingEntity = !isTrackingEntity;
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

