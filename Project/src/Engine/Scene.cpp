#include "./Scene.h"

#include "Renderer/Renderer.h"
#include "Entity.h"

Scene::Scene(Renderer* renderer) : renderer(renderer)
{
}

Scene::~Scene()
{
	entities.clear();
}

Entity* Scene::AddEntity(std::unique_ptr<Entity> entity)
{
	Entity* ptr = entity.get();           
	entities.push_back(std::move(entity));
	return ptr;                         
}

void Scene::RemoveEntity(std::unique_ptr<Entity> entity)
{
	entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
}

void Scene::Update(float deltaTime)
{
	if (updateFunction)
		updateFunction(deltaTime);
}

void Scene::Render() const
{
	// First: render normal entities
	for (const auto& e : entities)
	{
		if (!e->isGUI)
			renderer->RenderEntity(*e);
	}

	// Then: render GUI entities
	for (const auto& e : entities)
	{
		if (e->isGUI)
			renderer->RenderEntity(*e);
	}
}
