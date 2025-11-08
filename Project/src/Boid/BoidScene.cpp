#include "./BoidScene.h"

#include <glm/glm.hpp>

#include <iostream>

#include "../Engine/Renderer/TextureManager.h"
#include <cstdlib>

BoidScene::BoidScene(Renderer* renderer) : Scene(renderer)
{
	backgroundColor = glm::vec3(0.75f, 1.0f, 1.0f);
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
		float speed = static_cast<float>((rand() % 50) + 50); // Speed between 50 and 100
		float angle = static_cast<float>(rand() % 360);
		glm::vec2 velocity = glm::vec2(cosf(glm::radians(angle)), sinf(glm::radians(angle))) * speed;
		auto boid = std::make_unique<Boid>(glm::vec2(x, y), velocity);
		// Randomly assign to a scout group
		boid->group = (rand() % 4); // 0, 1 or 2
		boid->bias = 0.05f + static_cast<float>(rand()) / RAND_MAX * 0.05f; // Small bias
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
	return avgVel - boid->velocity; // Steering
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
			steer += diff / dist; // Stronger when closer
		}
	}
	if (glm::length(steer) > 0)
	{
		steer = glm::normalize(steer) * maxSpeed - boid->velocity;
	}
	return steer;
}

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
			// Direction away from obstacle center
			glm::vec2 away = glm::normalize(boid->position - obs.center);

			// Strength increases as we get closer (penetration proportion)
			float penetration = (sqrt(avoidRadius) - sqrt(dist)) / sqrt(avoidRadius);
			glm::vec2 desired = away * (maxSpeed * penetration);

			// Steering = desired velocity - current velocity (consistent with other steering implementations)
			glm::vec2 localSteer = desired - boid->velocity;
			steer += localSteer;
		}
	}
	// Clamp steering magnitude similar to other forces
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
		if (speed > 0.0f) // Avoid division by zero
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

void BoidScene::InitClouds(uint16_t count)
{
	std::vector<glm::vec2> vertices = { {-0.5f,-0.5f}, {0.5f,-0.5f}, {0.5f,0.5f}, {-0.5f,0.5f} };
	std::vector<glm::vec3> colors = { {1,1,1}, {1,1,1}, {1,1,1}, {1,1,1} };
	std::vector<glm::vec2> texCoords = { {0,0}, {1,0}, {1,1}, {0,1} };
	std::vector<uint32_t> indices = { 0,1,2, 2,3,0 };
	std::shared_ptr<Mesh> quad = std::make_shared<Mesh>(vertices, colors, texCoords, indices);

	// Distribute clouds into 3 groups -> group 1 = High (farthest), 2 = Mid, 3 = Low (nearest)
	int highCount = count * 50 / 100; // 50%
	int midCount = count * 35 / 100; // 35%
	int lowCount = count - highCount - midCount; // remaining 15%

	// Add back-to-front: High (1) first, then Mid (2), then Low (3) last
	const int addOrder[3] = { 1, 2, 3 };
	const int groupCounts[3] = { highCount, midCount, lowCount };

	int cloudIndex = 0;
	for (int orderIdx = 0; orderIdx < 3; ++orderIdx)
	{
		int group = addOrder[orderIdx];
		int thisGroupCount = groupCounts[orderIdx];

		for (int i = 0; i < thisGroupCount; ++i)
		{
			// Randomize position and properties
			float x = leftBound + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (rightBound - leftBound);
			float y = bottomBound + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (topBound - bottomBound);

			float scaleMin, scaleMax, speedMin, speedMax, opacityMin, opacityMax;
			if (group == 1) { // High
				scaleMin = cloudMinScale[0]; scaleMax = cloudMaxScale[0];
				speedMin = cloudMinSpeed[0]; speedMax = cloudMaxSpeed[0];
				opacityMin = cloudMinOpacity[0]; opacityMax = cloudMaxOpacity[0];
			}
			else if (group == 2) { // Mid
				scaleMin = cloudMinScale[1]; scaleMax = cloudMaxScale[1];
				speedMin = cloudMinSpeed[1]; speedMax = cloudMaxSpeed[1];
				opacityMin = cloudMinOpacity[1]; opacityMax = cloudMaxOpacity[1];
			}
			else { // Low
				scaleMin = cloudMinScale[2]; scaleMax = cloudMaxScale[2];
				speedMin = cloudMinSpeed[2]; speedMax = cloudMaxSpeed[2];
				opacityMin = cloudMinOpacity[2]; opacityMax = cloudMaxOpacity[2];
			}

			float scale = scaleMin + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (scaleMax - scaleMin);
			float opacity = opacityMin + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (opacityMax - opacityMin);
			float speed = speedMin + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (speedMax - speedMin);

			// Random texture
			int texIndex = 1 + (rand() % 10);
			char path[128];
			snprintf(path, sizeof(path), "textures/cloud%02d.png", texIndex);
			auto tex = TextureManager::Load(path);

			auto cloudEntity = std::make_unique<Entity>(quad, glm::vec2(x, y), glm::vec2(scale, scale), 0.0f, tex);
			cloudEntity->useTexture = true;
			cloudEntity->transparency = opacity;

			Entity* rawPtr = cloudEntity.get();
			AddEntity(std::move(cloudEntity));
			clouds.push_back({ rawPtr, speed, group });
			++cloudIndex;
		}
	}
}

void BoidScene::UpdateClouds(float deltaTime)
{
	for (auto& c : clouds)
	{
		if (!c.entity) continue;
		c.entity->position.x += c.speed * deltaTime;

		// Wrapped behavior: when cloud goes beyond rightBound, place it at leftBound with new random Y and group-specific properties
		if (c.entity->position.x - (c.entity->scale.x * 0.5f) > rightBound + 1.0f)
		{
			c.entity->position.x = leftBound - 5.0f; // Place far to the left to avoid pop-ins upon reapperance
			c.entity->position.y = bottomBound + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (topBound - bottomBound);

			// Re-apply group-specific ranges
			int group = c.group;
			float scaleMin, scaleMax, speedMin, speedMax, opacityMin, opacityMax;
			if (group == 1) { // High
				scaleMin = cloudMinScale[0]; scaleMax = cloudMaxScale[0];
				speedMin = cloudMinSpeed[0]; speedMax = cloudMaxSpeed[0];
				opacityMin = cloudMinOpacity[0]; opacityMax = cloudMaxOpacity[0];
			}
			else if (group == 2) { // Mid
				scaleMin = cloudMinScale[1]; scaleMax = cloudMaxScale[1];
				speedMin = cloudMinSpeed[1]; speedMax = cloudMaxSpeed[1];
				opacityMin = cloudMinOpacity[1]; opacityMax = cloudMaxOpacity[1];
			}
			else { // Low
				scaleMin = cloudMinScale[2]; scaleMax = cloudMaxScale[2];
				speedMin = cloudMinSpeed[2]; speedMax = cloudMaxSpeed[2];
				opacityMin = cloudMinOpacity[2]; opacityMax = cloudMaxOpacity[2];
			}

			// Randomize new properties within the group's ranges
			c.speed = speedMin + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (speedMax - speedMin);
			c.entity->transparency = opacityMin + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (opacityMax - opacityMin);
			float newScale = scaleMin + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (scaleMax - scaleMin);
			c.entity->scale = glm::vec2(newScale, newScale);

			// Randomize texture
			int texIndex = 1 + (rand() % 10);
			char path[128];
			snprintf(path, sizeof(path), "textures/cloud%02d.png", texIndex);
			c.entity->SetTexture(TextureManager::Load(path));
		}
	}
}

void BoidScene::ClearClouds()
{
	clouds.clear();
	// Entities vector still owns the cloud Entities; user can decide to clear them if needed.
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