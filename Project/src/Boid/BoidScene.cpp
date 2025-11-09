#include "./BoidScene.h"

#include <glm/glm.hpp>

#include <iostream>
#include <algorithm>

#include "../Engine/Renderer/Renderer.h"
#include "../Engine/Renderer/TextureManager.h"
#include "../Engine/InputManager.h"

BoidScene::BoidScene(Renderer* renderer) : Scene(renderer)
{
	backgroundColor = glm::vec3(0.375f, 0.7f, 1.0f);
	updateFunction = [this](float deltaTime)
		{
			UpdateBoids(deltaTime);
			UpdateClouds(deltaTime);
		};
}

BoidScene::~BoidScene()
{
}

void BoidScene::InitBoids(uint16_t count)
{
	//randomly add boids withing the scene bounds with random velocities and rotations
	for (uint16_t i = 0; i < count; ++i)
	{
		float x = static_cast<float>(rand() % (10*BOID_SCENE_W)) / 10.0f - BOID_SCENE_W / 2.0f;
		float y = static_cast<float>(rand() % (10*BOID_SCENE_H)) / 10.0f - BOID_SCENE_H / 2.0f;
		float speed = static_cast<float>((rand() % 25) + 25); // Speed between 25 and 50
		float angle = static_cast<float>(rand() % 360);
		glm::vec2 velocity = glm::vec2(cosf(glm::radians(angle)), sinf(glm::radians(angle))) * speed;
		auto boid = std::make_unique<Boid>(glm::vec2(x, y), velocity);
		boidEntities.push_back(boid.get());
		AddEntity(std::move(boid));

		// assign each boid to a random nest
		for (auto boid : boidEntities) {
			boidNestMap[boid] = rand() % nestEntities.size();
		}
	}
}

void BoidScene::InitNests(uint16_t count)
{
	//randomly add nests within the scene bounds
	for (uint16_t i = 0; i < count; ++i)
	{
		float x = static_cast<float>(rand() % (2 * (BOID_SCENE_W-1))) / 2.0f - (BOID_SCENE_W-1) / 2.0f;
		float y = static_cast<float>(rand() % (2 * (BOID_SCENE_H-1))) / 2.0f - (BOID_SCENE_H-1) / 2.0f;
		auto nest = std::make_unique<Nest>(glm::vec2(x, y));
		nestEntities.push_back(nest.get());
		AddEntity(std::move(nest));
	}
}

void BoidScene::InitClouds(uint16_t count)
{
	//randomly add clouds within the scene bounds
	for (uint16_t i = 0; i < count; ++i)
	{
		float x = static_cast<float>(rand() % (10 * BOID_SCENE_W)) / 5.0f - BOID_SCENE_W;
		float y = static_cast<float>(rand() % (10 * (BOID_SCENE_H+5))) / 10.0f - (BOID_SCENE_H+5) / 2.0f;
		float scale = static_cast<float>((rand() % 50) + 50) / 10.0f; // Scale between 5 and 10.0
		float opacity = static_cast<float>((rand() % 50) + 50) / 100.0f; // Opacity between 0.1 and 1.0
		float height = static_cast<float>((rand() % 50) + 50) / 50.0f; // Height between 1.0 and 2.0

		auto cloud = new Cloud(glm::vec2(x, y), glm::vec2(scale, scale), opacity, height, leftBound*2, rightBound*2);
		cloudEntities.push_back(cloud);
	}

	// sort clouds by height so that higher clouds are rendered later
	std::sort(cloudEntities.begin(), cloudEntities.end(),
		[](const Cloud* a, const Cloud* b) {
			return a->getHeight() < b->getHeight();
		});
	for (auto& cloud : cloudEntities) {
		AddEntity(std::move(std::unique_ptr<Cloud>(cloud)));
	}
}

std::shared_ptr<Mesh> BoidScene::CreateBoidMesh()
{
	// Define a simple triangular mesh for the boid
	std::vector<glm::vec2> vertices = {
		{ 0.0f,  0.5f},
		{-0.5f, -0.5f},
		{ 0.0f, 0.0f},
		{ 0.5f, -0.5f },
	};
	std::vector<glm::vec3> colors = {
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{ 1.0f, 1.0f, 1.0f }
	};
	std::vector<glm::vec2> texCoords = {
		{ 0.5f,  1.0f},
		{0.0f, 0.0f},
		{ 0.5f, 0.5f},
		{ 1.0f, 0.0f },
	};
	std::vector<uint32_t> indices = { 0, 1, 2, 0, 3, 2 };
	return std::make_shared<Mesh>(vertices, colors, texCoords, indices);
}

std::shared_ptr<Mesh> BoidScene::CreateNestMesh()
{
	std::vector<glm::vec2> vertices = {
		{-0.5f,  0.0f},
		{ 0.5f,  0.0f},
		{ 0.3f, -0.25f},
		{-0.3f, -0.25f},
		{-0.4f,  0.0f},
		{-0.3f, 0.25f},
		{-0.2f, 0.25f},
		{-0.1f, 0.0f},
		{ 0.2f, 0.0f},
		{ 0.25f, 0.125f},
		{ 0.35f, 0.125f},
		{ 0.4f, 0.0f},

	};
	std::vector<glm::vec3> colors = {
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
	};
	std::vector<glm::vec2> texCoords = {
		{0.0f, 0.5f},
		{1.0f, 0.5f},
		{0.8f, 0.25f},
		{0.2f, 0.25f},
		{0.1f, 0.5f},
		{0.2f, 0.75f},
		{0.3f, 0.75f},
		{0.4f, 0.5f},
		{0.15f, 0.625f},
		{0.2f, 0.75f},
		{0.3f, 0.75f},
		{0.35f, 0.625f},
	};
	std::vector<uint32_t> indices = { 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11 };
	return std::make_shared<Mesh>(vertices, colors, texCoords, indices);
}

void BoidScene::AddBackground(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture)
{
	backgroundEntity = std::make_shared<Entity>(mesh, glm::vec2(0.0f), glm::vec2(BOID_SCENE_W, BOID_SCENE_H), 0.0f, texture);
	backgroundEntity->useTexture = true;
	AddEntity(std::unique_ptr<Entity>(backgroundEntity.get()));
}

void BoidScene::AddControlEntities(std::shared_ptr<Mesh> mesh)
{
	auto sliderShader = renderer->GetShader("slider");
	//get textures
	auto alignTex = TextureManager::Load("textures/alignment.png");
	auto cohTex = TextureManager::Load("textures/cohesion.png");
	auto sepTex = TextureManager::Load("textures/separation.png");

	float sliderWidth = 2.0f / 3.0f;
	// add 3 rectangles (scaled from a square mesh) at the bottom of the scene to control alignment, cohesion, and separation weights
	alignmentControl = new Slider(mesh, glm::vec2(-sliderWidth, -0.9f), glm::vec2(sliderWidth, 0.2f), 0.0f, alignTex, sliderShader, true, 0.0f, 2.0f);
	cohesionControl = new Slider(mesh, glm::vec2(0.0f, -0.9f), glm::vec2(sliderWidth, 0.2f), 0.0f, cohTex, sliderShader, true, 0.0f, 2.0f);
	separationControl = new Slider(mesh, glm::vec2(sliderWidth, -0.9f), glm::vec2(sliderWidth, 0.2f), 0.0f, sepTex, sliderShader, true, 0.0f, 2.0f);

	//controls for sliders
	alignmentControl->BindControl(alignmentWeight, '[', ']');
	cohesionControl->BindControl(cohesionWeight, ';', '\'');
	separationControl->BindControl(separationWeight, ',', '.');

	AddEntity(std::unique_ptr<Entity>(alignmentControl));
	AddEntity(std::unique_ptr<Entity>(cohesionControl));
	AddEntity(std::unique_ptr<Entity>(separationControl));
}

std::vector<Boid*> BoidScene::GetNearbyBoids(const Boid* boid)
{
	std::vector<Boid*> neighbors;
	for (const auto& entity : boidEntities)
	{
		if (entity != boid)
		{
			float dist = glm::length(entity->position - boid->position);
			if (dist < neighborRadius)
				neighbors.push_back(entity);
		}
	}
	return neighbors;
}

glm::vec2 BoidScene::ComputeAlignment(const Boid* boid, const std::vector<Boid*>& neighbors)
{
	if (neighbors.empty()) return glm::vec2(0.0f);
	glm::vec2 avgVel(0.0f);
	for (auto n : neighbors) avgVel += n->velocity;
	avgVel /= static_cast<float>(neighbors.size());
	avgVel = glm::normalize(avgVel) * maxSpeed;
	return avgVel - boid->velocity; // steering
}

glm::vec2 BoidScene::ComputeCohesion(const Boid* boid, const std::vector<Boid*>& neighbors)
{
	if (neighbors.empty()) return glm::vec2(0.0f);
	glm::vec2 center(0.0f);
	for (auto n : neighbors) center += n->position;
	center /= static_cast<float>(neighbors.size());
	glm::vec2 desired = center - boid->position;
	desired = glm::normalize(desired) * maxSpeed;
	return desired - boid->velocity;
}

glm::vec2 BoidScene::ComputeSeparation(const Boid* boid, const std::vector<Boid*>& neighbors)
{
	glm::vec2 steer(0.0f);
	for (auto n : neighbors)
	{
		float dist = glm::length(boid->position - n->position);
		if (dist < separationRadius && dist > 0)
		{
			glm::vec2 diff = glm::normalize(boid->position - n->position);
			steer += diff / dist; // stronger when closer
		}
	}
	if (glm::length(steer) > 0)
	{
		steer = glm::normalize(steer) * maxSpeed - boid->velocity;
	}
	return steer;
}

glm::vec2 BoidScene::ComputeNestAttraction(const Boid* boid)
{
	int nestIndex = boidNestMap[boid];
	Nest* nest = nestEntities[nestIndex];

	// Attraction towards nest center
	glm::vec2 toNest = nest->position - boid->position;
	float dist = glm::length(toNest);

	glm::vec2 attract(0.0f);
	if (dist > 0.01f) {
		attract = glm::normalize(toNest) * attractStrength;

		//  if inside nest radius, orbit around it instead of heading straight in
		if (dist < nestRadius) {
			glm::vec2 tangent = glm::normalize(glm::vec2(-toNest.y, toNest.x)); // perpendicular
			attract += tangent * (attractStrength * 0.5f); // swirl effect
		}
	}
	return attract;
}

void BoidScene::ApplyEdgeAvoidance(Boid* boid, float deltaTime)
{
	float distance;

	// Left edge
	distance = (boid->position.x - leftBound);
	if (distance < edgeMargin)
		boid->velocity.x += turnFactor * deltaTime * (1.0f - distance / edgeMargin);

	// Right edge
	distance = (rightBound - boid->position.x);
	if (distance < edgeMargin)
		boid->velocity.x -= turnFactor * deltaTime * (1.0f - distance / edgeMargin);

	// Bottom edge
	distance = (boid->position.y - bottomBound);
	if (distance < edgeMargin)
		boid->velocity.y += turnFactor * deltaTime * (1.0f - distance / edgeMargin);

	// Top edge
	distance = (topBound - boid->position.y);
	if (distance < edgeMargin)
		boid->velocity.y -= turnFactor * deltaTime * (1.0f - distance / edgeMargin);

}

void BoidScene::UpdateBoids(float deltaTime)
{
	timeSinceRetarget += deltaTime;
	if (timeSinceRetarget >= nestRetargetInterval) {
		timeSinceRetarget = 0.0f;

		// randomly reassign the nest of a random boid and its neighbors
		Entity* randomBoid = GetRandomBoid();
		if (randomBoid) {
			int newNestIndex = rand() % nestEntities.size();
			boidNestMap[static_cast<Boid*>(randomBoid)] = newNestIndex;
			auto neighbors = GetNearbyBoids(static_cast<Boid*>(randomBoid));
			for (auto neighbor : neighbors) {
				boidNestMap[neighbor] = newNestIndex;
			}
		}
	}


	for (auto& boid : boidEntities)
	{

		auto neighbors = GetNearbyBoids(boid);

		glm::vec2 align = ComputeAlignment(boid, neighbors) * alignmentWeight;
		glm::vec2 coh = ComputeCohesion(boid, neighbors) * cohesionWeight;
		glm::vec2 sep = ComputeSeparation(boid, neighbors) * separationWeight;
		glm::vec2 nestAttract = ComputeNestAttraction(boid);

		glm::vec2 accel = align + coh + sep + nestAttract;
		if (glm::length(accel) > maxForce)
			accel = glm::normalize(accel) * maxForce;

		boid->velocity += accel * deltaTime;

		ApplyEdgeAvoidance(boid, deltaTime);

		float speed = glm::length(boid->velocity);
		if (speed > 0.0f) // avoid division by zero
		{
			if (speed < minSpeed)
				boid->velocity = glm::normalize(boid->velocity) * minSpeed;
			else if (speed > maxSpeed)
				boid->velocity = glm::normalize(boid->velocity) * maxSpeed;
		}

		boid->position += boid->velocity * deltaTime;
		boid->rotation = VectorToAngle(boid->velocity);

	}
}

void BoidScene::UpdateClouds(float deltaTime)
{
	for (auto& cloud : cloudEntities)
	{
		cloud->Update(deltaTime);
	}
}

Entity* BoidScene::GetRandomBoid()
{
	if (boidEntities.empty()) return nullptr;
	int index = rand() % boidEntities.size();
	return boidEntities[index];
}

Entity* BoidScene::GetRandomNest()
{
	if (nestEntities.empty()) return nullptr;
	int index = rand() % nestEntities.size();
	return nestEntities[index];
}
