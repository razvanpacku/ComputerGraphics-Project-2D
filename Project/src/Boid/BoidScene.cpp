#include "./BoidScene.h"

#include <glm/glm.hpp>

#include <iostream>

BoidScene::BoidScene(Renderer* renderer) : Scene(renderer)
{
	backgroundColor = glm::vec3(0.75f, 1.0f, 1.0f);
	updateFunction = [this](float deltaTime)
		{
			UpdateBoids(deltaTime);
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
		float speed = static_cast<float>((rand() % 50) + 50); // Speed between 50 and 100
		float angle = static_cast<float>(rand() % 360);
		glm::vec2 velocity = glm::vec2(cosf(glm::radians(angle)), sinf(glm::radians(angle))) * speed;
		auto boid = std::make_unique<Boid>(glm::vec2(x, y), velocity);
		// randomly assign to a scout group
		boid->group = (rand() % 4); // 0, 1 or 2
		boid->bias = 0.05f + static_cast<float>(rand()) / RAND_MAX * 0.05f; // small bias
		AddEntity(std::move(boid));
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

void BoidScene::AddBackground(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture)
{
	backgroundEntity = std::make_shared<Entity>(mesh, glm::vec2(0.0f), glm::vec2(BOID_SCENE_W, BOID_SCENE_H), 0.0f, texture);
	backgroundEntity->useTexture = true;
	AddEntity(std::unique_ptr<Entity>(backgroundEntity.get()));
}

std::vector<Boid*> BoidScene::GetNearbyBoids(const Boid* boid)
{
	std::vector<Boid*> neighbors;
	for (const auto& entity : entities)
	{
		Boid* other = dynamic_cast<Boid*>(entity.get());
		if (other && other != boid)
		{
			float dist = glm::length(other->position - boid->position);
			if (dist < neighborRadius)
				neighbors.push_back(other);
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

// New: compute obstacle avoidance steering
glm::vec2 BoidScene::ComputeObstacleAvoidance(const Boid* boid)
{
	glm::vec2 steer(0.0f);
	for (const auto& obs : obstacles)
	{
		glm::vec2 toCenter = obs.center - boid->position;
		float dist = glm::dot(toCenter, toCenter);
		float avoidRadius = (obs.radius + obstacleAvoidDistance) * (obs.radius + obstacleAvoidDistance);

		// If inside avoid radius, push away proportional to penetration
		if (dist < avoidRadius && dist > 0.0001f)
		{
			// direction away from obstacle center
			glm::vec2 away = glm::normalize(boid->position - obs.center);

			// strength increases as we get closer (penetration proportion)
			float penetration = (sqrt(avoidRadius) - sqrt(dist)) / sqrt(avoidRadius);
			glm::vec2 desired = away * (maxSpeed * penetration);

			// steering = desired velocity - current velocity (consistent with other steering implementations)
			glm::vec2 localSteer = desired - boid->velocity;
			steer += localSteer;
		}
	}
	// clamp steering magnitude similar to other forces
	if (glm::length(steer) > maxForce)
		steer = glm::normalize(steer) * maxForce;
	return steer;
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
	for (auto& entity : entities)
	{
		Boid* boid = dynamic_cast<Boid*>(entity.get());
		if (!boid) continue;

		auto neighbors = GetNearbyBoids(boid);

		glm::vec2 align = ComputeAlignment(boid, neighbors) * alignmentWeight;
		glm::vec2 coh = ComputeCohesion(boid, neighbors) * cohesionWeight;
		glm::vec2 sep = ComputeSeparation(boid, neighbors) * separationWeight;

		glm::vec2 accel = align + coh + sep;
		if (glm::length(accel) > maxForce)
			accel = glm::normalize(accel) * maxForce;

		boid->velocity += accel * deltaTime;

		ApplyEdgeAvoidance(boid, deltaTime);

		if (boid->group == 1) {
			if (boid->velocity.x > 0.0f)
				boid->bias = std::min(max_bias, boid->bias + bias_increment);
			else
				boid->bias = std::max(bias_increment, boid->bias - bias_increment);
		}
		if (boid->group == 2) {
			if (boid->velocity.x < 0.0f)
				boid->bias = std::min(max_bias, boid->bias + bias_increment);
			else
				boid->bias = std::max(bias_increment, boid->bias - bias_increment);
		}

		if (boid->group == 1) {
			// Biased to the right
			boid->velocity.x = (1.0f - boid->bias) * boid->velocity.x + (boid->bias * 1.0f);
		}
		else if (boid->group == 2) {
			// Biased to the left
			boid->velocity.x = (1.0f - boid->bias) * boid->velocity.x - boid->bias;
		}

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

Entity* BoidScene::GetRandomBoid()
{
	std::vector<Boid*> boids;
	for (const auto& entity : entities)
	{
		Boid* boid = dynamic_cast<Boid*>(entity.get());
		if (boid)
			boids.push_back(boid);
	}
	if (boids.empty()) return nullptr;
	int index = rand() % boids.size();
	return boids[index];
}

// Public obstacle API
void BoidScene::AddObstacle(const glm::vec2& center, float radius)
{
	obstacles.push_back({ center, radius });
}

void BoidScene::ClearObstacles()
{
	obstacles.clear();
}