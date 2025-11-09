#pragma once

#include "../Engine/Scene.h"
#include "Boid.h"
#include "Nest.h"
#include "Cloud.h"
#include "Slider.h"

#include <unordered_map>

//forard declarations
class Mesh;
class Texture;

#define BOID_SCENE_W 20
#define BOID_SCENE_H 20

class BoidScene : public Scene
{
public:
	BoidScene(Renderer* renderer);
	~BoidScene();

	void InitBoids(uint16_t count);
	void InitNests(uint16_t count);
	void InitClouds(uint16_t count);

	static std::shared_ptr<Mesh> CreateBoidMesh();
	static std::shared_ptr<Mesh> CreateNestMesh();

	//override update function
	void SetUpdateFunction(const std::function<void(float)>& func) = delete;
		
	void AddBackground(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture);
	void AddControlEntities(std::shared_ptr<Mesh> mesh);

	void ToggleMouseAttractAt(const glm::vec2& worldPos);
	void SetMouseAttractPos(const glm::vec2& worldPos);
	bool IsMouseAttractActive() const { return mouseAttractActive; }

	Entity* GetRandomBoid();
	Entity* GetRandomNest();
private:
	// Scene boundaries
	float leftBound = -BOID_SCENE_W / 2.0f;
	float rightBound = BOID_SCENE_W / 2.0f;
	float topBound = BOID_SCENE_H / 2.0f;
	float bottomBound = -BOID_SCENE_H / 2.0f;

	// Edge avoidance parameters
	float edgeMargin = 2.0f;      // Distance from edge before turning
	float turnFactor = 25.0f;     // How sharply boids turn near edges

	// Simulation parameters
	float neighborRadius = 1.0f;     // How far a boid can "see"
	float separationRadius = 0.5f;   // Minimum distance before repelling
	float minSpeed = 0.5f;            // Minimum speed
	float maxSpeed = 5.0f;           // Speed cap
	float maxForce = 5.0f;           // Steering force limit
	float alignmentWeight = 1.0f;
	float cohesionWeight = 0.4f;
	float separationWeight = 1.5f;

	float bias_increment = 0.0005f; // How much bias increases per update
	float max_bias = 0.05f;        // Maximum bias value

	float nestRadius = 0.5f;
	float nestAttractStrength = 3.0f;
	float nestRetargetInterval = 5.0f;  // seconds between random reassignments
	float timeSinceRetarget = 0.0f;
	std::unordered_map<const Boid*, int> boidNestMap; // which nest each boid is bound to


	// Entities
	std::vector<Boid*> boidEntities;
	std::vector<Nest*> nestEntities;
	std::vector<Cloud*> cloudEntities;
	std::shared_ptr<Entity> backgroundEntity;
	Slider* alignmentControl, * cohesionControl, * separationControl;

	bool mouseAttractActive = false;
	glm::vec2 mouseAttractPos = glm::vec2(0.0f);
	float mouseAttractRadius = 7.5f;
	float mouseAttractStrength = 15.0f;

	// Internal helpers
	std::vector<Boid*> GetNearbyBoids(const Boid* boid);
	glm::vec2 ComputeAlignment(const Boid* boid, const std::vector<Boid*>& neighbors);
	glm::vec2 ComputeCohesion(const Boid* boid, const std::vector<Boid*>& neighbors);
	glm::vec2 ComputeSeparation(const Boid* boid, const std::vector<Boid*>& neighbors);
	glm::vec2 ComputeNestAttraction(const Boid* boid);
	glm::vec2 ComputeMouseAttraction(const Boid* boid);
	void ApplyEdgeAvoidance(Boid* boid, float deltaTime);

	// Update logic
	void UpdateBoids(float deltaTime);
	void UpdateClouds(float deltaTime);
};

