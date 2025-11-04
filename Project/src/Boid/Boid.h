#pragma once

#include "../Engine/Entity.h"

class Boid : public Entity
{
public:
	Boid(const glm::vec2& position = glm::vec2(0.0f), const glm::vec2& velocity = glm::vec2(0.0f));
	~Boid();

	glm::vec2 velocity;

	static void InitSharedResources(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture);
private:
	static std::shared_ptr<Mesh> sharedMesh;
	static std::shared_ptr<Texture> sharedTexture;
};

float VectorToAngle(const glm::vec2& vec);