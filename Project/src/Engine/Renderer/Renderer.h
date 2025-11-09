#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Shader.h"
#include "../Scene.h"
#include "Mesh.h"
#include "../Entity.h"
#include "Camera.h"

#include <unordered_map>

//forward declarations
class App;

#define DEFAULT_CLEAR_COLOR_R 0.75f
#define DEFAULT_CLEAR_COLOR_G 1.0f
#define DEFAULT_CLEAR_COLOR_B 1.0f


class Renderer
{
public:
	Renderer(App* app);
	~Renderer();

	void Init();

	void Clear() const;

	std::shared_ptr<Mesh> AddMesh(const std::string& name, std::shared_ptr<Mesh> mesh);
	std::shared_ptr<Mesh> GetMesh(const std::string& name) const;

	std::shared_ptr<Shader> AddShader(const std::string& name, const std::string& vertPath, const std::string& fragPath);
	std::shared_ptr<Shader> GetShader(const std::string& name) const;

	void Update() const;
	
	void RenderEntity(const Entity& entity) const;
	void Render() const;

	void Cleanup() const;

	void SetScene(Scene* newScene) { scene = newScene; }
	Scene* GetScene() const { return scene; }
	void SetCamera(Camera* newCamera) { camera = newCamera; }
	Camera* GetCamera() const { return camera; }

private:
	App* app;

	std::unordered_map<std::string, std::shared_ptr<Mesh>> meshCache;
	std::unordered_map<std::string, std::shared_ptr<Shader>> shaderCache;

	Shader* baseShader;

	Camera* camera = nullptr;

	Scene* scene = nullptr;

	//temporary static variables, move to another file later
	void CreateShaders(void);
};

