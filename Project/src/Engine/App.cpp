#include "./App.h"

#include "Renderer/TextureManager.h"
#include "InputManager.h"
#include "../Boid/Boid.h"
#include "../Boid/BoidScene.h"

#include <cmath>
#include <ctime>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
	//camera controls
	InputManager::RegisterMouseWheelAction([camera](int direction) {
		camera->ProcessMouseScroll(direction);
		});
	InputManager::RegisterKeyAction('t', [this]() {
		this->SetEntityTracking(false);
		});
	InputManager::RegisterKeyAction('y', [this]() {
		this->SetEntityTracking(true);
		});
	InputManager::RegisterMouseButtonAction([this](int button, int state, int x, int y) {
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			// Convert screen (x,y) to world coordinates
			Camera* cam = renderer->GetCamera();
			if (!cam) return;

			// Flip Y
			float winX = static_cast<float>(x);
			float winY = static_cast<float>(window->GetHeight() - y);

			glm::vec4 viewport(0.0f, 0.0f, static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()));

			// glm::unProject expects model (modelview) and proj matrices.
			// Pass view matrix as the "model" param
			glm::vec3 winPos(winX, winY, 0.0f);

			glm::vec3 worldPos = glm::unProject(winPos, cam->GetViewMatrix(), cam->GetProjectionMatrix(), viewport);

			// Toggle mouse attract in the BoidScene
			Scene* sc = renderer->GetScene();
			if (sc)
			{
				BoidScene* bs = dynamic_cast<BoidScene*>(sc);
				if (bs)
				{
					bs->ToggleMouseAttractAt(glm::vec2(worldPos.x, worldPos.y));
				}
			}
		}
		});

	// Update attract point continuously while mouse-attract mode is active
	InputManager::RegisterCursorPosAction([this](int x, int y) {
		Camera* cam = renderer->GetCamera();
		if (!cam) return;

		float winX = static_cast<float>(x);
		float winY = static_cast<float>(window->GetHeight() - y);
		glm::vec4 viewport(0.0f, 0.0f, static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()));
		glm::vec3 winPos(winX, winY, 0.0f);
		glm::vec3 worldPos = glm::unProject(winPos, cam->GetViewMatrix(), cam->GetProjectionMatrix(), viewport);

		Scene* sc = renderer->GetScene();
		if (sc)
		{
			BoidScene* bs = dynamic_cast<BoidScene*>(sc);
			if (bs && bs->IsMouseAttractActive())
			{
				bs->SetMouseAttractPos(glm::vec2(worldPos.x, worldPos.y));
			}
		}
		});

	//Load textures
	auto tex1 = TextureManager::Load("textures/dev.png");
	auto nestTex = TextureManager::Load("textures/nest.png");
	auto boidTex = TextureManager::Load("textures/boid.png");
	auto backgroundTex = TextureManager::Load("textures/background.png");
	TextureManager::Load("textures/alignment.png");
	TextureManager::Load("textures/cohesion.png");
	TextureManager::Load("textures/separation.png");

	// Define a square mesh
	std::vector<glm::vec2> vertices = { {-0.5f,-0.5f}, {0.5f,-0.5f}, {0.5f,0.5f}, {-0.5f,0.5f} };
	std::vector<glm::vec3> colors = { {1,1,1}, {1,1,1}, {1,1,1}, {1,1,1} };
	std::vector<glm::vec2> texCoords = { {0,0}, {1,0}, {1,1}, {0,1} };
	std::vector<uint32_t> indices = { 0,1,2, 2,3,0 };
	//
	auto squareMesh = renderer->AddMesh("square", std::make_shared<Mesh>(vertices, colors, texCoords, indices));

	Scene* scene = new BoidScene(renderer);

	auto nestMesh = renderer->AddMesh("nest", BoidScene::CreateNestMesh());
	Nest::InitSharedResources(nestMesh, nestTex);
	auto boidMesh = renderer->AddMesh("boid", BoidScene::CreateBoidMesh());
	Boid::InitSharedResources(boidMesh, boidTex);

	std::vector<std::shared_ptr<Texture>> cloudTextures = {
		TextureManager::Load("textures/cloud1.png"),
		TextureManager::Load("textures/cloud2.png"),
		TextureManager::Load("textures/cloud3.png"),
		TextureManager::Load("textures/cloud4.png"),
	};
	Cloud::InitSharedResources(squareMesh, cloudTextures);

	dynamic_cast<BoidScene*>(scene)->AddBackground(squareMesh, backgroundTex);
	dynamic_cast<BoidScene*>(scene)->InitNests(5);
	dynamic_cast<BoidScene*>(scene)->InitBoids(500);
	dynamic_cast<BoidScene*>(scene)->InitClouds(20);
	dynamic_cast<BoidScene*>(scene)->AddControlEntities(squareMesh);

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

void App::SetEntityTracking(bool boidOrNest)
{
	isTrackingEntity = !isTrackingEntity;

	//special behaviour for boidscene
	if (isTrackingEntity){
		if(boidOrNest){
			trackedEntity = dynamic_cast<BoidScene*>(renderer->GetScene())->GetRandomNest();
		}
		else{
			trackedEntity = dynamic_cast<BoidScene*>(renderer->GetScene())->GetRandomBoid();
		}
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

