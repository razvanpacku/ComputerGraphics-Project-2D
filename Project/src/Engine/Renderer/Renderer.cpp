#include "./Renderer.h"

//temporary functions for rendering, move to another file later

#include "Util/loadShaders.h"

#include "../App.h"
#include "../InputManager.h"

void Renderer::CreateShaders(void)
{
	AddShader("slider", "shaders/slider.vert", "shaders/slider.frag");
	auto _baseShader = AddShader("base", "shaders/base.vert", "shaders/base.frag");
	baseShader = _baseShader.get();
}

Renderer::Renderer(App* app) : app(app)
{
	Init();
}

Renderer::~Renderer()
{
	meshCache.clear();
	shaderCache.clear();
}

void Renderer::Init()
{
	static Renderer* self = (Renderer*)this;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

	glutIdleFunc([]() {
		glutPostRedisplay(); 
		});

	glutCloseFunc([]() {
		self->Cleanup();
		});

	glutKeyboardFunc([](unsigned char key, int x, int y) {
		if (key == 27) { // ESC key
			glutLeaveMainLoop();
		}
		InputManager::OnKeyPress(key);
		});


	glutMouseFunc([](int button, int state, int x, int y) {
		InputManager::OnMouseButton(button, state, x, y);
		});

	glutMotionFunc([](int x, int y) {
		InputManager::OnCursorPos(x, y);
		});

	glutPassiveMotionFunc([](int x, int y) {
		InputManager::OnCursorPos(x, y);
		});

	glutMouseWheelFunc([](int wheel, int direction, int x, int y) {
		InputManager::OnMouseWheel(direction);
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
	return meshCache[name];
}

std::shared_ptr<Mesh> Renderer::GetMesh(const std::string& name) const
{
	auto it = meshCache.find(name);
	if (it != meshCache.end()) {
		return it->second;
	}
	return nullptr;
}

std::shared_ptr<Shader> Renderer::AddShader(const std::string& name, const std::string& vertPath, const std::string& fragPath)
{
	if (shaderCache.find(name) == shaderCache.end()) {
		auto sh = std::make_shared<Shader>(vertPath.c_str(), fragPath.c_str());
		shaderCache[name] = sh;
		return sh;
	}
	return shaderCache[name];
}

std::shared_ptr<Shader> Renderer::GetShader(const std::string& name) const
{
	auto it = shaderCache.find(name);
	if (it != shaderCache.end())
		return it->second;
	return nullptr;
}

void Renderer::Update() const
{
	app->Update();
}

void Renderer::RenderEntity(const Entity& entity) const
{
	std::shared_ptr<Shader> useShader = entity.GetShader();
	if (!useShader)
		useShader = std::shared_ptr<Shader>(baseShader, [](Shader*) {}); // non-owning reference

	useShader->Use();

	if (entity.isGUI)
	{
		// GUI
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);

		useShader->setMat4("view", view);
		useShader->setMat4("projection", proj);
	}
	else
	{
		// Regular world-space entity
		useShader->setMat4("view", camera->GetViewMatrix());
		useShader->setMat4("projection", camera->GetProjectionMatrix());
	}

	entity.ApplyUniforms(*useShader);

	//draw the entity
	entity.Draw();
}

void Renderer::Render() const
{
	if (!camera || !scene) return;

	Clear();

	//do rendering here
	scene->Render();

	glFlush();
}

void Renderer::Cleanup() const
{
	//cleanup resources here
}