#include "./Renderer.h"

//temporary functions for rendering, move to another file later

#include "Util/loadShaders.h"

#include "../App.h"

void Renderer::CreateShaders(void)
{
	shader = new Shader("shaders/base.vert", "shaders/base.frag");
	shader->Use();
}

Renderer::Renderer(App* app) : app(app)
{
	Init();

	activeShader = shader;
}

Renderer::~Renderer()
{
	meshCache.clear();
	delete shader;
}

void Renderer::Init()
{
	static Renderer* self = (Renderer*)this;

	if (scene) {
		glClearColor(scene->backgroundColor.r, scene->backgroundColor.g, scene->backgroundColor.b, 1.0f);
	} else {
		glClearColor(DEFAULT_CLEAR_COLOR_R, DEFAULT_CLEAR_COLOR_G, DEFAULT_CLEAR_COLOR_B, 1.0f);
	}

	glutDisplayFunc([]() {
		self->Update();
		self->Render();

		glutSwapBuffers();
		});
	glutCloseFunc([]() {
		self->Cleanup();
		});

	glutIdleFunc([]() {
		glutPostRedisplay(); 
		});

	glutKeyboardFunc([](unsigned char key, int x, int y) {
		if (key == 27) { // ESC key
			glutLeaveMainLoop();
		}
		else if (key == 't') {
			//track entity
			self->app->SetEntityTracking();
		}
		});


	glutMouseWheelFunc([](int wheel, int direction, int x, int y) {
		self->camera->ProcessMouseScroll(direction);
		});

	CreateShaders();
}

void Renderer::Clear() const
{
	if (scene) {
		glClearColor(scene->backgroundColor.r, scene->backgroundColor.g, scene->backgroundColor.b, 1.0f);
	}
	else {
		glClearColor(DEFAULT_CLEAR_COLOR_R, DEFAULT_CLEAR_COLOR_G, DEFAULT_CLEAR_COLOR_B, 1.0f);
	}
	glClear(GL_COLOR_BUFFER_BIT);
}

std::shared_ptr<Mesh> Renderer::AddMesh(const std::string& name, std::shared_ptr<Mesh> mesh)
{
	if (meshCache.find(name) == meshCache.end()) {
		meshCache[name] = mesh;
		return mesh;
	}
	return nullptr;
}

std::shared_ptr<Mesh> Renderer::GetMesh(const std::string& name) const
{
	auto it = meshCache.find(name);
	if (it != meshCache.end()) {
		return it->second;
	}
	return nullptr;
}

void Renderer::Update() const
{
	app->Update();
}

void Renderer::RenderEntity(const Entity& entity) const
{
	//set up shader uniforms here
	activeShader->setMat4("model", entity.GetModelMatrix());
	activeShader->setBool("useTexture", entity.useTexture);
	//draw the entity
	entity.Draw();
}

void Renderer::Render() const
{
	if (!camera || !scene) return;

	Clear();

	activeShader->Use();

	glm::mat4 view = camera->GetViewMatrix();
	glm::mat4 proj = camera->GetProjectionMatrix();
	activeShader->setMat4("view", view);
	activeShader->setMat4("projection", proj);

	//do rendering here
	scene->Render();

	glFlush();
}

void Renderer::Cleanup() const
{
	//cleanup resources here
}